#include "physxclass.h"

//Defining a custome filter shader 
PxFilterFlags customFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	 //all initial and persisting reports for everything, with per-point data
	//pairFlags = PxPairFlag::eCONTACT_DEFAULT
	//	| PxPairFlag::eTRIGGER_DEFAULT
	//	| PxPairFlag::eNOTIFY_CONTACT_POINTS
	//	| PxPairFlag::eCCD_LINEAR; //Set flag to enable CCD (Continuous Collision Detection) 

	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}



	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	//if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	//	return PxFilterFlag::eCALLBACK;

	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	{
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

		return PxFilterFlag::eDEFAULT;
	}

	return PxFilterFlag::eKILL;

}

struct FilterGroup
{
	enum Enum
	{
		eHand = (1 << 0),
		eWall = (1 << 1),
		eFinger = (1 << 4),
		eBox = (1 << 2),
	};
};

long long PhysxClass::milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}

void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a contact callback;
	const PxU32 numShapes = actor->getNbShapes();
	PxShape* shapes[2];
	actor->getShapes(shapes, 2);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
}


PhysxClass::PhysxClass()
{
	moveAbleWall = nullptr;
	activeActor = nullptr;
}

PhysxClass::~PhysxClass()
{

}

bool PhysxClass::Initialize()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

	static PxProfileZoneManager *mProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(gFoundation);

	gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());
	if (gPhysicsSDK == NULL) {
		return false;
	}

	if (!PxInitExtensions(*gPhysicsSDK))
		return false;

	if (gPhysicsSDK->getPvdConnectionManager() == NULL)
		return false;
	const char* pvd_host_ip = "127.0.0.1";
	int port = 5425;
	unsigned int timeout = 100;


	//--- Debugger
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	theConnection = PxVisualDebuggerExt::createConnection(gPhysicsSDK->getPvdConnectionManager(),
		pvd_host_ip, port, timeout, connectionFlags);


	//Create the scene
	PxSceneDesc sceneDesc(gPhysicsSDK->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);

	if (!sceneDesc.cpuDispatcher) {
		PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(4);
		if (!mCpuDispatcher)
			return false;
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}
	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = customFilterShader;//gDefaultFilterShader;
	//sceneDesc.simulationEventCallback = this;
	sceneDesc.simulationEventCallback = this;
	sceneDesc.flags |=  PxSceneFlag::eENABLE_KINEMATIC_PAIRS ;
	//
	//PxCudaContextManagerDesc cudaContextManagerDesc;
	//PxCudaContextManager* mCudaContextManager = PxCreateCudaContextManager(*gFoundation, cudaContextManagerDesc, mProfileZoneManager);
	//if (mCudaContextManager) {
	//	if (!mCudaContextManager->contextIsValid()) {
	//		mCudaContextManager->release();
	//		mCudaContextManager = NULL;

	//		exit(EXIT_FAILURE);
	//	}

	//	if (!sceneDesc.gpuDispatcher) {
	//		sceneDesc.gpuDispatcher = mCudaContextManager->getGpuDispatcher();
	//	}

	//}


	gScene = gPhysicsSDK->createScene(sceneDesc);
	if (!gScene)
		return false;

	gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);


	mMaterial = gPhysicsSDK->createMaterial(0.5, 0.5, 0.5);

	//Create actors 
	//1) Create ground plane
	PxReal d = 0.0f;
	PxQuat flatGround = PxQuat(PxHalfPi, PxVec3(0, 0, 1.0f));
	PxQuat slant = PxQuat(7 * PxPi / 180, PxVec3(1, 0, 0));
	PxQuat slantFloor = slant*flatGround;
	PxTransform pose = PxTransform(PxVec3(0.0f, 0.0f, 0.0f), slantFloor);

	moveAbleFloor = gPhysicsSDK->createRigidStatic(pose);
	if (!moveAbleFloor)
		return false;

	PxShape* shape = moveAbleFloor->createShape(PxPlaneGeometry(), *mMaterial);
	if (!shape)
		return false;


	setupFiltering(moveAbleFloor, FilterGroup::eWall, FilterGroup::eBox);
	gScene->addActor(*moveAbleFloor);


	pose = PxTransform(PxVec3(-3.0f, 0.0f, 0.0f), PxQuat(0.0, PxVec3(0.0f, 1.0f, 0.0f)));
	PxRigidStatic* plane = gPhysicsSDK->createRigidStatic(pose);
	shape = plane->createShape(PxPlaneGeometry(), *mMaterial);

	setupFiltering(plane, FilterGroup::eWall, FilterGroup::eBox);
	gScene->addActor(*plane);


	pose = PxTransform(PxVec3(3.0f, 0.0f, 0.0f), PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f)));
	plane = gPhysicsSDK->createRigidStatic(pose);
	shape = plane->createShape(PxPlaneGeometry(), *mMaterial);

	setupFiltering(plane, FilterGroup::eWall, FilterGroup::eBox);
	gScene->addActor(*plane);


	pose = PxTransform(PxVec3(0.0f, 0.0f, -0.5f), PxQuat(PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f)));
	moveAbleWall = gPhysicsSDK->createRigidStatic(pose);
	shape = moveAbleWall->createShape(PxPlaneGeometry(), *mMaterial);

	setupFiltering(moveAbleWall, FilterGroup::eWall, FilterGroup::eBox);
	gScene->addActor(*moveAbleWall);

	pose = PxTransform(PxVec3(0.0f, 0.0f, -6.0f), PxQuat(-PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f)));
	plane = gPhysicsSDK->createRigidStatic(pose);
	shape = plane->createShape(PxPlaneGeometry(), *mMaterial);

	setupFiltering(plane, FilterGroup::eWall, FilterGroup::eBox);
	gScene->addActor(*plane);


	////2)           Create cube	 
	//PxReal         density = 1.0f;
	//PxTransform    transform(PxVec3(0.0f, 10.0f, 0.0f), PxQuat::createIdentity());
	//PxVec3         dimensions(BoxSize,BoxSize,BoxSize);
	//PxBoxGeometry  geometry(dimensions);

	//for (int i = 0; i < 5; i++)
	//{
	//	transform.p = PxVec3(0.0f, 0.0f+2.0f*i, -3.0f);
	//	PxRigidDynamic *actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);
	//	//setupFiltering(actor, FilterGroup::eBox, FilterGroup::ePARTICLE);
	//	actor->setAngularDamping(0.75);
	//	//actor->setMass(0.1);
	//	actor->setLinearVelocity(PxVec3(0, 0, 0));
	//	actor->setSolverIterationCounts(16, 8);
	//	if (!actor)
	//		cerr << "create actor failed!" << endl;
	//	gScene->addActor(*actor);
	//	boxes.push_back(actor);
	//	//PxDistanceJoint* joint = PxDistanceJointCreate(*gPhysicsSDK, actor, PxTransform(PxVec3(0.0f, 0.0f, 0.0f)), pole, PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));
	//	//joint->setMaxDistance(2.0f);
	//	//joint->setDistanceJointFlag(PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
	//	//joint->setDamping(1.0f);
	//	//joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
	//}

	//for (int i = 0; i < 5; i++)
	//{
	//	transform.p = PxVec3(-2.0f + i, 0.0f, -3.0f);
	//	PxRigidDynamic *actor = PxCreateDynamic(*gPhysicsSDK, transform, PxSphereGeometry(BoxSize), *mMaterial, density);
	//	//setupFiltering(actor, FilterGroup::eBox, FilterGroup::ePARTICLE);

	//	//actor->setMass(0.1);
	//	actor->setLinearVelocity(PxVec3(0, 0, 0));
	//	actor->setSolverIterationCounts(16, 8);
	//	if (!actor)
	//		cerr << "create actor failed!" << endl;
	//	gScene->addActor(*actor);
	//	spheres.push_back(actor);
	//}
	start_time = milliseconds_now();

	return true;
}

void PhysxClass::Render()
{
	long long timeSinceStart = milliseconds_now() - start_time;
	float deltaTime = (timeSinceStart - oldTimeSinceStart) / 1000.0f;
	oldTimeSinceStart = timeSinceStart;

	//activeActor = nullptr;
	//activeHand = nullptr;
	mAccumulator += deltaTime;

	while (mAccumulator > myTimestep) //Simulate at not more than 'gTimeStep' time-interval 
	{
		mAccumulator -= myTimestep;
		StepPhysx();
	}
}

void PhysxClass::applyForce(std::map<int, PxVec3> forces)
{

	for (map<int, PxVec3>::iterator ii = forces.begin(); ii != forces.end(); ++ii)
	{
		if (gTouchFound == true)
		{
			if ((*ii).second == PxVec3(0, 0, 0))
			{
				activeContact[(*ii).first] = nullptr;
				activeContact.erase((*ii).first);

			}
			else
				activeContact[(*ii).first]->addForce((*ii).second);
		}
	}

	if (activeContact.size() == 0)
		gTouchFound = false;
}

PxRigidDynamic* PhysxClass::createBox(PxVec3 dimension, PxVec3 pose, PxQuat quat)
{
	PxReal density = 1.0f;
	PxTransform transform(pose, quat);
	PxBoxGeometry geometry(dimension);

	PxRigidDynamic* actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);
	actor->setSolverIterationCounts(16, 8);
	//actor->setMass(1.0);
	//actor->setMassSpaceInertiaTensor(PxVec3(1.0, 1.0, 1.0));
	setupFiltering(actor, FilterGroup::eBox, FilterGroup::eBox | FilterGroup::eFinger | FilterGroup::eWall | FilterGroup::eHand);
	gScene->addActor(*actor);

	return actor;

}
PxRigidDynamic* PhysxClass::createSphere(PxReal radius, PxVec3 pose, PxQuat quat)
{
	PxReal density = 1.0f;
	PxTransform transform(pose, quat);
	PxSphereGeometry geometry(radius);

	PxRigidDynamic* actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);
	actor->setSolverIterationCounts(16, 8);
	//actor->setMass(1.0);
	//actor->setMassSpaceInertiaTensor(PxVec3(1.0, 1.0, 1.0));
	setupFiltering(actor, FilterGroup::eBox, FilterGroup::eBox | FilterGroup::eFinger | FilterGroup::eWall | FilterGroup::eHand);
	gScene->addActor(*actor);

	return actor;
}

PxRigidStatic* PhysxClass::createStaticSphere(PxReal radius, PxVec3 pose, PxQuat quat)
{
	PxReal density = 1.0f;
	PxTransform transform(pose, quat);
	PxSphereGeometry geometry(radius);
	
	PxRigidStatic* actor = gPhysicsSDK->createRigidStatic(transform);
	actor->createShape(PxSphereGeometry(radius), *mMaterial);
	gScene->addActor(*actor);

	return actor;
}

PxRigidDynamic* PhysxClass::createCapsule(PxReal radius, PxReal halfHeight, PxVec3 pose, PxQuat quat)
{
	PxReal density = 1.0f;
	PxTransform transform(pose, quat);
	PxCapsuleGeometry geometry(radius, halfHeight);

	PxRigidDynamic* actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);
	actor->setSolverIterationCounts(16, 8);
	//setupFiltering(actor, FilterGroup::eBox, FilterGroup::eBox | FilterGroup::eFinger | FilterGroup::eWall | FilterGroup::eHand);
	gScene->addActor(*actor);

	return actor;
}



void PhysxClass::removeActor(PxRigidActor* actor)
{
	gScene->removeActor(*actor);
	actor->release();
}

PxPhysics* PhysxClass::getPhysicsSDK()
{
	return gPhysicsSDK;
}

PxScene* PhysxClass::getScene()
{
	return gScene;
}

void PhysxClass::StepPhysx()
{
	gScene->simulate(myTimestep);
	while (!gScene->fetchResults())
	{

	}
}

void PhysxClass::Shutdown()
{
	if (gScene != NULL)
	{
		//for (int i = 0; i < boxes.size(); i++)
		//	gScene->removeActor(*boxes[i]);

		gScene->release();

		//for (int i = 0; i < boxes.size(); i++)
		//	boxes[i]->release();
	}
	if (gPhysicsSDK != NULL)
		gPhysicsSDK->release();
}

void PhysxClass::moveWall(float zoffset)
{
	moveAbleWall->setGlobalPose(PxTransform(PxVec3(0.0f, 0.0f, zoffset), PxQuat(PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f))));
	PxQuat flatGround = PxQuat(PxHalfPi, PxVec3(0, 0, 1.0f));
	PxQuat slant = PxQuat(7 * PxPi / 180, PxVec3(1, 0, 0));
	PxQuat slantFloor = slant*flatGround;
	PxTransform pose = PxTransform(PxVec3(0.0f, 0.0f, zoffset), slantFloor);

	moveAbleFloor->setGlobalPose(pose);
}


void PhysxClass::setHandActor(std::vector<handActor> HandList)
{
	mHandList = HandList;
}
int PhysxClass::getActiveHandID()
{
	return gTouchId;
}
PxRigidDynamic* PhysxClass::getActiveActor()
{
	return activeActor;
}

std::map<int, PxRigidDynamic*> PhysxClass::getActiveContact()
{
	return activeContact;
}




void PhysxClass::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	//for (PxU32 i = 0; i < nbPairs; i++)
	//{
	//	const PxContactPair& cp = pairs[i];

	//	if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
	//	{
	//			PxFilterData filterData_1 = cp.shapes[0]->getSimulationFilterData();
	//			PxFilterData filterData_2 = cp.shapes[1]->getSimulationFilterData();
	//			if (filterData_1.word0 == FilterGroup::eHand && filterData_2.word0 == FilterGroup::eBox)
	//			{
	//				for (int j = 0; j < mHandList.size(); j++)
	//				{
	//					if ((PxRigidDynamic*)pairHeader.actors[0] == mHandList[j].palm)
	//					{
	//						gTouchId = mHandList[j].id;
	//						activeActor = (PxRigidDynamic*)pairHeader.actors[1];
	//						gTouchFound = true;
	//					}
	//				}
	//			}
	//	}
	//}

	for (PxU32 i = 0; i < count; i++)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
			PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;


		if (pairs[i].otherShape->getGeometryType() == PxGeometryType::eBOX || pairs[i].otherShape->getGeometryType() == PxGeometryType::eSPHERE) 
		{
			
				for (int j = 0; j < mHandList.size(); j++)
				{
					if (pairs[i].triggerActor == mHandList[j].palm)
					{
						activeContact[mHandList[j].id] = (PxRigidDynamic*)pairs[i].otherActor;

						//gTouchId = mHandList[j].id;
						//activeActor = (PxRigidDynamic*)pairs[i].otherActor;
						gTouchFound = true;
					}
				}
		}
	}
}


