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
		pairFlags = PxPairFlag::eSOLVE_CONTACT;
		pairFlags |= PxPairFlag::eDETECT_DISCRETE_CONTACT;
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
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
		eTarget = (1 << 3),
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
		shape->setQueryFilterData(filterData);
	}
}


PhysxClass::PhysxClass()
{
	moveAbleWall = nullptr;
	activeActor = nullptr;
	gGrabFound = true;
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

	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));
	if (!gCooking)
		return false;

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
	sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
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
	gPhysicsSDK->getVisualDebugger()->setVisualDebuggerFlags
	(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS | PxVisualDebuggerFlag::eTRANSMIT_CONSTRAINTS);
	gScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);

	mMaterial = gPhysicsSDK->createMaterial(1.0,1.0,0.5);

	//Create actors 
	//1) Create ground plane
	PxReal d = 0.0f;
	PxQuat flatGround = PxQuat(PxHalfPi, PxVec3(0, 0, 1.0f));
	PxQuat slant = PxQuat(7 * PxPi / 180, PxVec3(1, 0, 0));
	PxQuat slantFloor = slant*flatGround;
	PxTransform pose = PxTransform(PxVec3(0.0f, 0.5, 0.0f), flatGround);

	moveAbleFloor = gPhysicsSDK->createRigidStatic(pose);
	if (!moveAbleFloor)
		return false;

	PxShape* shape = moveAbleFloor->createShape(PxPlaneGeometry(), *mMaterial);
	if (!shape)
		return false;


	setupFiltering(moveAbleFloor, FilterGroup::eWall, FilterGroup::eBox | FilterGroup::eTarget);
	gScene->addActor(*moveAbleFloor);


	pose = PxTransform(PxVec3(-2.5f, FLOOR_LEVEL, 0.0f), PxQuat(0.0, PxVec3(0.0f, 1.0f, 0.0f)));
	PxRigidStatic* plane = gPhysicsSDK->createRigidStatic(pose);
	shape = plane->createShape(PxPlaneGeometry(), *mMaterial);

	setupFiltering(plane, FilterGroup::eWall, FilterGroup::eBox | FilterGroup::eTarget);
	gScene->addActor(*plane);


	pose = PxTransform(PxVec3(2.5f, FLOOR_LEVEL, 0.0f), PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f)));
	plane = gPhysicsSDK->createRigidStatic(pose);
	shape = plane->createShape(PxPlaneGeometry(), *mMaterial);

	setupFiltering(plane, FilterGroup::eWall, FilterGroup::eBox | FilterGroup::eTarget);
	gScene->addActor(*plane);


	pose = PxTransform(PxVec3(0.0f, 0.0f, 5.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f)));
	moveAbleWall = gPhysicsSDK->createRigidStatic(pose);
	shape = moveAbleWall->createShape(PxPlaneGeometry(), *mMaterial);

	setupFiltering(moveAbleWall, FilterGroup::eWall, FilterGroup::eBox | FilterGroup::eTarget);
	gScene->addActor(*moveAbleWall);

	pose = PxTransform(PxVec3(0.0f, 0.0f, -5.0f), PxQuat(-PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f)));
	moveAbleFrontWall = gPhysicsSDK->createRigidStatic(pose);
	shape = moveAbleFrontWall->createShape(PxPlaneGeometry(), *mMaterial);

	setupFiltering(moveAbleFrontWall, FilterGroup::eWall, FilterGroup::eBox | FilterGroup::eTarget);
	gScene->addActor(*moveAbleFrontWall);


	//PxTriangleMesh* cup = createTriangleMesh("cup._obj");
	//PxMeshScale scale(PxVec3(1,1,1), PxQuat(PxIdentity));
	//PxTriangleMeshGeometry geom(cup, scale);
	//PxBoxGeometry box(1, 1, 1);

	////PxRigidActor* myActor = gPhysicsSDK->createRigidDynamic(PxTransform(PxVec3(0, 5, -2)));
	////shape = myActor->createShape(geom, *mMaterial);
	//PxTransform relativePose(PxVec3(0, 5, -2));
	//PxRigidDynamic* aCapsuleActor = gPhysicsSDK->createRigidDynamic(PxTransform(relativePose));
	//
	//PxShape* aCapsuleShape = aCapsuleActor->createShape(geom,
	//	*mMaterial);
	//PxRigidBodyExt::updateMassAndInertia(*aCapsuleActor, 1.0f);
	//gScene->addActor(*aCapsuleActor);

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
				activeContact[(*ii).first]->setLinearVelocity((*ii).second);
		}
	}

	if (activeContact.size() == 0)
		gTouchFound = false;
}

void PhysxClass::clearContact()
{
	if (gTouchFound == true)
	{	
		//for (map<int, PxRigidDynamic*>::iterator it = activeContact.begin(); it != activeContact.end(); it++) {
		//	removeActor(it->second);
		//	it->second = nullptr;
		//}
		activeContact.clear();

	}
}

PxRigidDynamic* PhysxClass::createBox(PxVec3 dimension, PxVec3 pose, PxQuat quat)
{
	PxReal density = 1.0f;
	PxTransform transform(pose, quat);
	PxBoxGeometry geometry(dimension);
	//mMaterial = gPhysicsSDK->createMaterial(0, 0, 0.5);
	PxRigidDynamic* actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);
	actor->setSolverIterationCounts(32, 255);
	//actor->setMass(1.0);
	//actor->setMassSpaceInertiaTensor(PxVec3(1.0, 1.0, 1.0));
	setupFiltering(actor, FilterGroup::eBox, FilterGroup::eBox | FilterGroup::eFinger | FilterGroup::eWall | FilterGroup::eHand | FilterGroup::eTarget );
	actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	PxU32 nShapes = actor->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	actor->getShapes(shapes, nShapes);
	int* test = (int*)malloc(sizeof(int));
	*test = 0;
	shapes[0]->userData = test;
	*(int*)shapes[0]->userData = 0;
	gScene->addActor(*actor);

	return actor;

}

PxRigidActor* PhysxClass::createBarrier(PxVec3 dimension, PxVec3 pose, PxQuat quat)
{
	PxReal density = 1.0f;
	PxTransform transform(pose, quat);
	PxBoxGeometry geometry(dimension);

	PxRigidActor* actor = PxCreateStatic(*gPhysicsSDK, transform, geometry,*mMaterial);
	//actor->setMass(1.0);
	//actor->setMassSpaceInertiaTensor(PxVec3(1.0, 1.0, 1.0));
	setupFiltering(actor, FilterGroup::eWall, FilterGroup::eBox | FilterGroup::eFinger | FilterGroup::eWall | FilterGroup::eHand | FilterGroup::eTarget);
	gScene->addActor(*actor);

	return actor;

}

PxRigidActor* PhysxClass::createTarget(PxVec3 dimension, PxVec3 pose, PxQuat quat)
{
	PxReal density = 1.0f;
	PxTransform transform(pose, quat);
	PxBoxGeometry geometry(dimension);

	PxRigidStatic* actor = PxCreateStatic(*gPhysicsSDK, transform, geometry, *mMaterial);
	//actor->setMass(1.0);
	//actor->setMassSpaceInertiaTensor(PxVec3(1.0, 1.0, 1.0));
	setupFiltering(actor, FilterGroup::eTarget, FilterGroup::eBox | FilterGroup::eWall);
	gScene->addActor(*actor);

	return actor;

}

PxRigidDynamic* PhysxClass::createSphere(PxReal radius, PxVec3 pose, PxQuat quat)
{
	PxReal density = 1.0f;
	PxTransform transform(pose, quat);
	PxSphereGeometry geometry(radius);

	PxRigidDynamic* actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);
	actor->setSolverIterationCounts(32, 255);
	//actor->setMass(1.0);
	//actor->setMassSpaceInertiaTensor(PxVec3(1.0, 1.0, 1.0));
	setupFiltering(actor, FilterGroup::eBox, FilterGroup::eBox | FilterGroup::eFinger | FilterGroup::eWall | FilterGroup::eHand | FilterGroup::eTarget);
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
	setupFiltering(actor, FilterGroup::eBox, FilterGroup::eBox | FilterGroup::eFinger | FilterGroup::eWall | FilterGroup::eHand);
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

void PhysxClass::moveBackWall(float zoffset)
{
	moveAbleWall->setGlobalPose(PxTransform(PxVec3(0.0f, FLOOR_LEVEL, -0.5+zoffset), PxQuat(PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f))));
	PxQuat flatGround = PxQuat(PxHalfPi, PxVec3(0, 0, 1.0f));
	//PxQuat slant = PxQuat(7 * PxPi / 180, PxVec3(1, 0, 0));
	//PxQuat slantFloor = slant*flatGround;
	PxTransform pose = PxTransform(PxVec3(0.0f, FLOOR_LEVEL, -0.5+zoffset), flatGround);

	moveAbleFloor->setGlobalPose(pose);
}

void PhysxClass::moveFloor(float yoffset)
{
	PxTransform wall = moveAbleWall->getGlobalPose();
	wall.p.y = yoffset;
	moveAbleWall->setGlobalPose(wall);
	PxTransform floor = moveAbleFloor->getGlobalPose();
	floor.p.y = yoffset;

	moveAbleFloor->setGlobalPose(floor);
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

bool PhysxClass::getTargetStatus()
{
	return gTargetHit;
}

void PhysxClass::clearTargetStatus()
{
	gTargetHit = false;
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
			if (pairs[i].otherActor->isRigidDynamic())
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
}

void PhysxClass::onContact(const PxContactPairHeader& pairHeader,
	const PxContactPair* pairs, PxU32 nbPairs)
{

	const PxU32 buff = 64; //buffer size
	PxContactPairPoint contacts[buff];
	std::vector<PxContactPairPoint> contactPoints;

	//loop through all contact pairs of PhysX simulation
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		//extract contant info from current contact-pair 
		const PxContactPair& curContactPair = pairs[i];
		if (curContactPair.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			PxFilterData filterData_1 = curContactPair.shapes[0]->getSimulationFilterData();
			PxFilterData filterData_2 = curContactPair.shapes[1]->getSimulationFilterData();
			if (filterData_1.word0 == FilterGroup::eBox && filterData_2.word0 == FilterGroup::eTarget ||
				filterData_1.word0 == FilterGroup::eTarget && filterData_2.word0 == FilterGroup::eBox)
			{
				if (pairs->shapes[1]->getGeometryType() == PxGeometryType::eBOX && pairs->shapes[0]->getGeometryType() == PxGeometryType::eBOX)
				{
					gTargetHit = true;
				}
			}
			else if (filterData_1.word0 == FilterGroup::eBox && filterData_2.word0 == FilterGroup::eFinger ||
				filterData_1.word0 == FilterGroup::eFinger && filterData_2.word0 == FilterGroup::eBox)
			{
				if (curContactPair.flags & PxContactPairFlag::eACTOR_PAIR_HAS_FIRST_TOUCH)
				{
					if (!(curContactPair.flags & PxContactPairFlag::eDELETED_SHAPE_0) && !(curContactPair.flags & PxContactPairFlag::eREMOVED_SHAPE_0))
					{
						if(pairs->shapes[0]->getGeometryType() == PxGeometryType::eBOX)
						{
							if ((int*)pairs->shapes[0]->userData == NULL)
							{
								int* test = (int*)malloc(sizeof(int));
								pairs->shapes[0]->userData = test;
								*((int*)pairs->shapes[0]->userData) = 0;
							}
							else
							{
								*((int*)pairs->shapes[0]->userData) += 1;
							}
						}
					}
					if (!(curContactPair.flags & PxContactPairFlag::eDELETED_SHAPE_1) && !(curContactPair.flags & PxContactPairFlag::eREMOVED_SHAPE_1))
					{
						if (pairs->shapes[1]->getGeometryType() == PxGeometryType::eBOX)
						{
							if ((int*)pairs->shapes[1]->userData == NULL)
							{
								int* test = (int*)malloc(sizeof(int));
								pairs->shapes[1]->userData = test;
								*((int*)pairs->shapes[1]->userData) = 0;
							}
							else
							{
								*((int*)pairs->shapes[1]->userData) += 1;
							}
						}
					}


				}
			
			}

		}
		else if (curContactPair.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			if (!(curContactPair.flags & PxContactPairFlag::eDELETED_SHAPE_0) && !(curContactPair.flags & PxContactPairFlag::eREMOVED_SHAPE_0) &&
				!(curContactPair.flags & PxContactPairFlag::eDELETED_SHAPE_1) && !(curContactPair.flags & PxContactPairFlag::eREMOVED_SHAPE_1))
			{


				PxFilterData filterData_1 = curContactPair.shapes[0]->getSimulationFilterData();
				PxFilterData filterData_2 = curContactPair.shapes[1]->getSimulationFilterData();
				if (filterData_1.word0 == FilterGroup::eBox && filterData_2.word0 == FilterGroup::eFinger ||
					filterData_1.word0 == FilterGroup::eFinger && filterData_2.word0 == FilterGroup::eBox)
				{


					if (curContactPair.flags & PxContactPairFlag::eACTOR_PAIR_LOST_TOUCH)
					{
							if (pairs->shapes[0]->getGeometryType() == PxGeometryType::eBOX)
							{
								if ((int*)pairs->shapes[0]->userData == NULL)
								{
									int* test = (int*)malloc(sizeof(int));
									pairs->shapes[0]->userData = test;
									*((int*)pairs->shapes[0]->userData) = 0;
								}
								else
								{
									*((int*)pairs->shapes[0]->userData) -=1;
								}
							}
						
							if (pairs->shapes[1]->getGeometryType() == PxGeometryType::eBOX)
							{
								if ((int*)pairs->shapes[1]->userData == NULL)
								{
									int* test = (int*)malloc(sizeof(int));
									pairs->shapes[1]->userData = test;
									*((int*)pairs->shapes[1]->userData) = 0;
								}
								else
								{
									*((int*)pairs->shapes[1]->userData) -= 1;
								}
							}
						

					}
				}
			}

		}

	}
}
void PhysxClass::moveFrontWall(float z)
{
//	moveAbleFrontWall->setGlobalPose(PxTransform(PxVec3(0.0f, 0.0f, z), PxQuat(-PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f))));
}

bool PhysxClass::ReadFromObj(char* filename, int& vertexCount, int& faceCount)
{
	ifstream fin;
	char input;


	// Initialize the counts.
	vertexCount = 0;
	faceCount = 0;

	// Open the file.
	fin.open(filename);

	// Check if it was successful in opening the file.
	if (fin.fail() == true)
	{
		return false;
	}

	// Read from the file and continue to read until the end of the file is reached.
	fin.get(input);
	while (!fin.eof())
	{
		// If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') { vertexCount++; }
		}

		// If the line starts with 'f' then increment the face count.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ') { faceCount++; }
		}

		// Otherwise read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	return true;
}

bool PhysxClass::LoadDataStructures(char* filename, int vertexCount,  int faceCount, PxVec3 vertices[], PxU32 faces[] )
{
	ifstream fin;

	int vertexIndex, faceIndex;
	int skiped;
	char input, input2;
	ofstream fout;


	// Initialize the four data structures.
	vertices = new PxVec3[vertexCount];
	if (!vertices)
	{
		return false;
	}

	faces = new PxU32[faceCount*3];
	if (!faces)
	{
		return false;
	}

	// Initialize the indexes.
	vertexIndex = 0;
	faceIndex = 0;

	// Open the file.
	fin.open(filename);

	// Check if it was successful in opening the file.
	if (fin.fail() == true)
	{
		return false;
	}

	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			// Read in the vertices.
			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				// Invert the Z vertex to change to left hand system.
				//vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

		}

		// Read in the faces.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				fin >> faces[faceIndex] >> input2 >> skiped >> input2 >> skiped
					>> faces[faceIndex+1] >> input2 >> skiped >> input2 >> skiped
					>> faces[faceIndex+2] >> input2 >> skiped >> input2 >> skiped;
				faceIndex+=3;
			}
		}

		// Read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();



	return true;
}

PxTriangleMesh*  PhysxClass::createTriangleMesh(char* filename)
{
	PxTriangleMeshDesc meshDesc;
	int nbVerts, triCount;
	PxVec3* vertices =NULL;
	PxU32* faces = NULL;
	ifstream fin;

	int vertexIndex, faceIndex;
	int skiped;
	char input, input2;
	ReadFromObj(filename, nbVerts, triCount);
	vertices = new PxVec3[nbVerts];

	faces = new PxU32[triCount * 3];

	// Initialize the indexes.
	vertexIndex = 0;
	faceIndex = 0;

	// Open the file.
	fin.open(filename);

	// Check if it was successful in opening the file.
	if (fin.fail() == true)
	{
		return false;
	}

	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			// Read in the vertices.
			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				// Invert the Z vertex to change to left hand system.
				//vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

		}

		// Read in the faces.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				fin >> faces[faceIndex ] >> input2 >> skiped >> input2 >> skiped
					>> faces[faceIndex +1] >> input2 >> skiped >> input2 >> skiped
					>> faces[faceIndex +2] >> input2 >> skiped >> input2 >> skiped;
				faceIndex += 3;
			}
		}

		// Read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();


	ofstream fout;
	fout.open("model.txt");

	// Write out the file header that our model format uses.
	fout << "Vertex Count: " << (triCount * 3) << endl;
	fout << endl;
	fout << "Data:" << endl;
	fout << endl;

	int vIndex = 0, fIndex = 0;
	// Now loop through all the faces and output the three vertices for each face.
	for (int i = 0; i<nbVerts; i++)
	{
		fout << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << "\n";
	}

	for (int i = 0; i<triCount*3; i += 3)
	{
		faces[i] = faces[i] - 1;
		faces[i+1] = faces[i+1] - 1;
		faces[i+2] = faces[i+2] - 1;
		fout << faces[i] << " " << faces[i + 1] << " " << faces[i + 2] << "\n";
	}

	// Close the output file.
	fout.close();

	meshDesc.points.count = nbVerts;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = vertices;

	meshDesc.triangles.count = triCount;
	meshDesc.triangles.stride = 3*sizeof(PxU32);
	meshDesc.triangles.data = faces;



	PxDefaultMemoryOutputStream writeBuffer;
	bool status = gCooking->cookTriangleMesh(meshDesc, writeBuffer);
	if (!status)
		return NULL;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	return gPhysicsSDK->createTriangleMesh(readBuffer);
}