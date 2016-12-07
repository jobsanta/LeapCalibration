#include "leapclass.h"
using namespace Leap;

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


void LeapClass::setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
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

void LeapClass::connect()
{
	while (!controller.isConnected());
	
}

bool LeapClass::captureFingerTip( float * x, float *y , float*z)
{
	while (!controller.isConnected());
	Frame frame = controller.frame();
	if (frame.id() == lastFrameID_cap)
	{
		return false;

	}

	HandList hands = frame.hands();
	if (hands.isEmpty())
	{		
		lastFrameID_cap = frame.id();
		return false;


	}
	else if (!hands.isEmpty())
	{
		Hand hand = hands[0];
		FingerList fingers = hand.fingers();
		PxVec3 result = leapToWorld(fingers[2].tipPosition());
		*x = result.x;
		*y = result.y;
		*z = result.z;
	}

	lastFrameID_cap = frame.id();
	return true;
}


bool LeapClass::capturePalm(float*z)
{
	while (!controller.isConnected());
	Frame frame = controller.frame();
	if (frame.id() == lastFrameID_cap)
	{
		return false;

	}

	HandList hands = frame.hands();
	if (hands.isEmpty())
	{
		lastFrameID_cap = frame.id();
		return false;


	}
	else if (!hands.isEmpty())
	{
		Hand hand = hands[0];
		
		*z = -hand.palmPosition().z / 100 - 1.5;
	}

	lastFrameID_cap = frame.id();
	return true;
}

void LeapClass::processFrame(float x, float y, float z, float offset,float xScale, float yScale,float factor)
{
	while (!controller.isConnected());
	Frame frame = controller.frame();
	if (frame.id() == lastFrameID) return;

	x_head = x;
	y_head = y;
	z_head = z;

	HandList hands = frame.hands();
	//bool createHand = false;

	z_offset = offset;
	x_scale = xScale;
	y_scale = yScale;
	
	//FingerList fingers = hand.fingers();
	//Leap::PointableList pointables = frame.pointables();
	//Leap::FingerList fingers = frame.fingers();
	//Leap::ToolList tools = frame.tools();


	if (!hands.isEmpty() && mHandList.size() == 0)
	{
		for (int i = 0; i < hands.count(); i++)
			createHand(hands[i], factor);

		//createRefHand(hands[0]);
	}
	if (hands.isEmpty() && mHandList.size() > 0)
	{
		for (int i = 0; i < mHandList.size(); i++)
			deleteHand(mHandList[i]);

		mHandList.clear();
	}
	int hands_count = hands.count();
	if (hands_count != (int)mHandList.size())
	{
		if (hands.count() > (int)mHandList.size())
		{
			for (int i = 0; i < mHandList.size(); i++)
				updateHand(frame.hand(mHandList[i].id), mHandList[i]);

			for (int i = 0; i < hands.count(); i++)
			{
				bool tracked = false;
				for (int j = 0; j < mHandList.size() && tracked == false; j++)
				{
					if (hands[i].id() == mHandList[j].id)
					{
						tracked = true;
					}
				}
				if (tracked == false)
				{
					createHand(hands[i], factor);
				}
			}
		}
		else if (hands_count < (int)mHandList.size())
		{
			std::vector<int> tobeDelete;
			for (int i = 0; i < mHandList.size(); i++)
			{
				if (frame.hand(mHandList[i].id).isValid())
				{
					updateHand(frame.hand(mHandList[i].id), mHandList[i]);
				}
				else
				{
					deleteHand(mHandList[i]);
					tobeDelete.push_back(i);
				}
			}

			int size = tobeDelete.size();
			for (int i = 0; i < size; i++)
			{
				mHandList.erase(mHandList.begin() + (int)tobeDelete.back());
				tobeDelete.pop_back();
			}
			tobeDelete.clear();
		}
	}
	if (hands.count() == (int)mHandList.size() && hands.count() != 0)
	{
		for (int i = 0; i < (int)mHandList.size(); i++)
			updateHand(frame.hand(mHandList[i].id), mHandList[i]);
	}


	lastFrameID = frame.id();
}

PxRigidDynamic* LeapClass::CreateSphere(const PxVec3& pos, const PxReal radius, const PxReal density)
{
	PxTransform transform(pos, PxQuat::createIdentity());
	PxSphereGeometry geometry(radius);

	PxMaterial* mMaterial = gPhysicsSDK->createMaterial(1.0, 1.0, 0.1);

	PxRigidDynamic* actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);

	if (!actor)
		cerr << "create actor failed" << endl;
	//actor->setAngularDamping(0.75);
	actor->setLinearVelocity(PxVec3(0, 0, 0));
	gScene->addActor(*actor);
	return actor;
}

PxRigidDynamic* LeapClass::CreateBox(const PxVec3& pos, const PxReal radius, const PxReal density)
{
	PxTransform transform(pos, PxQuat::createIdentity());
	PxBoxGeometry geometry(PxVec3(radius,radius,radius));

	PxMaterial* mMaterial = gPhysicsSDK->createMaterial(1.0, 1.0, 0.1);

	PxRigidDynamic* actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);

	if (!actor)
		cerr << "create actor failed" << endl;
	//actor->setAngularDamping(0.75);
	actor->setLinearVelocity(PxVec3(0, 0, 0));
	gScene->addActor(*actor);
	return actor;
}

void LeapClass::InitPhysx(PxPhysics* sdk, PxScene* scene)
{
	gPhysicsSDK = sdk;
	gScene = scene;

	ltoworldtransform = PxMat44::createIdentity();
}

vector<PxRigidActor*> LeapClass::getProxyParticle()
{
	return proxyParticleActor;
}

vector<PxRigidActor*> LeapClass::getProxyJoint()
{
	return proxyParticleJoint;
}

PxVec3 LeapClass::leapToWorld(Leap::Vector bone_center)
{
	PxVec3 converted;
	converted.x = bone_center.x / 100.f;
	converted.y = bone_center.y / 100.f - 3.35f;
	converted.z = -bone_center.z / 100.f - 1.5f;

	converted.x = (converted.x)*z_head/ (z_head - converted.z);
	converted.y = (converted.y)*z_head / (z_head - converted.z);

	float xs = converted.x / 3.0f;
	float ys  = converted.y /1.7f;
	//converted.z = (converted.z - 0.1) / (-0.1*converted.z);

	converted.x = xs*(converted.z+z_offset + 8.0f) / x_scale;
	converted.y = (ys*(converted.z+ z_offset + 8.0f) + y_scale*1.5) / y_scale;
	converted.z = converted.z + z_offset;

	//converted.w = 1;

	//PxVec4 buffer = projectionMatrix.transform(converted);
	//PxVec4 result = viewMatrix.transform(buffer);


	//PxVec3 result;
	//result = ltoworldtransform.transform(converted);
	//

	return converted;//PxVec3(result.x/result.w,result.y/result.w,result.z/result.w);

}

void LeapClass::setProjectionMatrix(PxMat44 mat)
{
	projectionMatrix = mat;
}

void LeapClass::setViewMatrix(PxMat44 mat)
{
	viewMatrix = mat;
}

void LeapClass::setMatrix(PxMat44 mat)
{
	ltoworldtransform = mat;
}

//void LeapClass::createJoint(Vector bone)
//{
//	float maxDist = 0.0f;
//	float damping = 2.0;
//	float stifness = 100.0f;
//	PxRigidDynamic* newParticleActor = CreateSphere(leapToWorld(bone), particleSize, 1.0f);
//	newParticleActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
//
//	PxU32 nShapes = newParticleActor->getNbShapes();
//	PxShape** shapes = new PxShape*[nShapes];
//	newParticleActor->getShapes(shapes, nShapes);
//
//	while (nShapes--)
//	{
//		shapes[nShapes]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
//	}
//
//	PxRigidDynamic* newParticleActorJoint = CreateSphere(leapToWorld(bone), particleSize, 1.0f);
//	//newParticleActorJoint->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
//
//	((PxRigidBody*)newParticleActorJoint)->setMass(5);
//	newParticleActorJoint->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
//	newParticleActorJoint->setSolverIterationCounts(8, 4);
//	setupFiltering(newParticleActorJoint, FilterGroup::ePARTICLE, FilterGroup::eInteract | FilterGroup::ePARTICLE);
//
//
//	PxDistanceJoint* joint = PxDistanceJointCreate(*gPhysicsSDK, newParticleActor, PxTransform(PxVec3(0.0, 0.0, 0.0)), newParticleActorJoint, PxTransform(PxVec3(0.0, 0.0, 0.0)));
//	joint->setMaxDistance(maxDist);
//	joint->setDamping(damping);
//	joint->setStiffness(stifness);
//	joint->setDistanceJointFlag(PxDistanceJointFlag::eSPRING_ENABLED, true);
//	joint->setDistanceJointFlag(PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
//
//
//	proxyParticleActor.push_back(newParticleActor);
//	proxyParticleJoint.push_back(newParticleActorJoint);
//}

void LeapClass::createHand(Hand hand, float factor)
{

	//Create Palm
	float damping = 1.0f;
	float stifness = 100.0f;
	float maxDist = 0.0f;
	PxU32 nbActors = 21;
	float mass = 0.05f;
	PxVec3 I_tensor = PxVec3(0.05, 0.05f, 0.05f);
	bool selfCollision = false;
	handActor newHand;

	newHand.aggregate = gPhysicsSDK->createAggregate(nbActors, selfCollision);
	newHand.isRef = false;
	newHand.id = hand.id();
	Leap::Matrix handTransform = hand.basis();
	handTransform.origin = hand.palmPosition();

	handTransform = handTransform.rigidInverse();


	PxVec3 col1 = PxVec3(hand.basis().xBasis.x, hand.basis().xBasis.y, hand.basis().xBasis.z);
	PxVec3 col2 = PxVec3(hand.basis().yBasis.x, hand.basis().yBasis.y, hand.basis().yBasis.z);
	PxVec3 col3 = PxVec3(hand.basis().zBasis.x, hand.basis().zBasis.y, hand.basis().zBasis.z);
	PxMat33 flipmat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });
	if (hand.isLeft())
	{
		col1 = PxVec3(-hand.basis().xBasis.x, -hand.basis().xBasis.y, -hand.basis().xBasis.z);
		col2 = PxVec3(hand.basis().yBasis.x, hand.basis().yBasis.y, hand.basis().yBasis.z);
		col3 = PxVec3(hand.basis().zBasis.x, hand.basis().zBasis.y, hand.basis().zBasis.z);
	}

	newHand.grabStrength = hand.grabStrength();
	newHand.pinchStrength = hand.pinchStrength();

	PxQuat quat = PxQuat(flipmat*PxMat33(col1, col2, col3)*flipmat);

	PxMaterial* material = gPhysicsSDK->createMaterial(0.5f, 0.5f, 0.5f);
	newHand.palm = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(hand.palmPosition()),quat), PxBoxGeometry((hand.palmWidth() - hand.fingers()[0].width())/400, 0.1, hand.palmWidth() / 400 ), *material, 1.0f);
	if (newHand.palm == NULL)
	{
		return;
	}
	newHand.palm->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
	PxShape* shapes;
	newHand.palm->getShapes(&shapes, 1);
	shapes->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	shapes->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);


	setupFiltering(newHand.palm, FilterGroup::eHand, FilterGroup::eBox);

	newHand.aggregate->addActor(*newHand.palm);





	newHand.palmDimension = PxVec3(hand.palmWidth()/120.0f, hand.palmWidth()/100.0f, hand.palmWidth()/400.0f);

	int i = 0;
	Leap::FingerList fingers = hand.fingers();
	for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); fl++, i++)
	{
		Leap::Bone bone;
		Leap::Bone::Type boneType;

		if (i == 0) // threat thumb different
		{
			for (int b = 2; b < 4; b++)
			{
				boneType = static_cast<Leap::Bone::Type>(b);
				bone = (*fl).bone(boneType);

				Leap::Matrix bonebasis = bone.basis();


				newHand.halfHeight[i][b] = factor*bone.length() / 200.0f;
				newHand.fingerWidth[i][b] = factor*bone.width() / 100.0f;

				newHand.finger_actor[i][b] = createCylinder((*fl).width() / 200.0f, newHand.halfHeight[i][b], leapToWorld(bone.center()), newHand.aggregate);
				newHand.finger_actor[i][b]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
				newHand.finger_actor[i][b]->setMass(mass);
				newHand.finger_actor[i][b]->setMassSpaceInertiaTensor(I_tensor);
				setupFiltering(newHand.finger_actor[i][b], FilterGroup::eFinger, FilterGroup::eBox);

			}

			boneType = static_cast<Leap::Bone::Type>(1);
			bone = (*fl).bone(boneType);

			newHand.halfHeight[i][1] = factor*bone.length() / 200.0f;
			newHand.fingerWidth[i][1] = factor*bone.width() / 100.0f;
			newHand.finger_actor[i][1] = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(bone.center())),
				PxBoxGeometry(newHand.halfHeight[i][1], newHand.fingerWidth[i][1] / 4.0f, newHand.fingerWidth[i][1] / 2.0f), *material, 1.0f);

			newHand.finger_actor[i][1]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			newHand.finger_actor[i][1]->setMass(mass);
			newHand.finger_actor[i][1]->setMassSpaceInertiaTensor(I_tensor);
			setupFiltering(newHand.finger_actor[i][1], FilterGroup::eFinger, FilterGroup::eBox);
			newHand.aggregate->addActor(*newHand.finger_actor[i][1]);

			for (int j = 3; j >= 2; j--)
			{
				newHand.finger_joint[i][j] = PxD6JointCreate(*gPhysicsSDK, newHand.finger_actor[i][j - 1], PxTransform(PxVec3(-newHand.halfHeight[i][j - 1], 0.0, 0.0)), newHand.finger_actor[i][j], PxTransform(PxVec3(newHand.halfHeight[i][j], 0.0, 0.0)));
				//newHand.finger_joint[i][j]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
			}

			boneType = static_cast<Leap::Bone::Type>(1);
			bone = (*fl).bone(boneType);
			Leap::Vector attachPos = handTransform.transformPoint(bone.prevJoint());
			PxVec3 attachPosMM = PxVec3(attachPos.x / 100.0, attachPos.y / 100.0, -attachPos.z / 100.0);
			if (hand.isLeft())
			{
				attachPosMM = PxVec3(-attachPos.x / 100.0, attachPos.y / 100.0, -attachPos.z / 100.0);
			}
			newHand.finger_joint[i][1] = PxD6JointCreate(*gPhysicsSDK, newHand.palm, PxTransform(attachPosMM), newHand.finger_actor[i][1], PxTransform(PxVec3(newHand.halfHeight[i][1], 0.0, 0.0), PxQuat(-PxHalfPi, PxVec3(0, 1, 0))));

			//finger_joint[i][0]->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

			//newHand.finger_joint[i][1]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
			newHand.finger_joint[i][1]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
			newHand.finger_joint[i][1]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
		}
		else
		{
			for (int b = 1; b < 4; b++)
			{
				boneType = static_cast<Leap::Bone::Type>(b);
				bone = (*fl).bone(boneType);

				newHand.halfHeight[i][b] = factor*bone.length() / 200.0f;
				newHand.fingerWidth[i][b] = factor*bone.width() / 100.0f;

				newHand.finger_actor[i][b] = createCylinder((*fl).width() / 200.0f, newHand.halfHeight[i][b], leapToWorld(bone.center()), newHand.aggregate);
				newHand.finger_actor[i][b]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
				newHand.finger_actor[i][b]->setMass(mass);
				newHand.finger_actor[i][b]->setMassSpaceInertiaTensor(I_tensor);

				setupFiltering(newHand.finger_actor[i][b], FilterGroup::eFinger, FilterGroup::eBox);

			}

			boneType = static_cast<Leap::Bone::Type>(0);
			bone = (*fl).bone(boneType);

			newHand.halfHeight[i][0] = factor*bone.length() / 200.0f;
			newHand.fingerWidth[i][0] = factor*bone.width() / 100.0f;
			newHand.finger_actor[i][0] = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(bone.center())),
				PxBoxGeometry(newHand.halfHeight[i][0], newHand.fingerWidth[i][0] / 4.0f, newHand.fingerWidth[i][0] / 2.0f), *material, 1.0f);

			newHand.finger_actor[i][0]->setMass(mass);
			newHand.finger_actor[i][0]->setMassSpaceInertiaTensor(I_tensor);


			newHand.finger_actor[i][0]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			setupFiltering(newHand.finger_actor[i][0], FilterGroup::eFinger, FilterGroup::eBox);
			newHand.aggregate->addActor(*newHand.finger_actor[i][0]);

			for (int j = 3; j >= 1; j--)
			{
				newHand.finger_joint[i][j] = PxD6JointCreate(*gPhysicsSDK, newHand.finger_actor[i][j - 1], PxTransform(PxVec3(-newHand.halfHeight[i][j - 1], 0.0, 0.0)), newHand.finger_actor[i][j], PxTransform(PxVec3(newHand.halfHeight[i][j], 0.0, 0.0)));
				//newHand.finger_joint[i][j]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);

			}

			boneType = static_cast<Leap::Bone::Type>(0);
			bone = (*fl).bone(boneType);
			Leap::Vector attachPos = handTransform.transformPoint(bone.prevJoint());
			PxVec3 attachPosMM = PxVec3(attachPos.x / 100.0, attachPos.y / 100.0, -attachPos.z / 100.0);
			if (hand.isLeft())
			{
				attachPosMM = PxVec3(-attachPos.x / 100.0, attachPos.y / 100.0, -attachPos.z / 100.0);
			}
			newHand.finger_joint[i][0] = PxD6JointCreate(*gPhysicsSDK, newHand.palm, PxTransform(attachPosMM), newHand.finger_actor[i][0], PxTransform(PxVec3(newHand.halfHeight[i][0], 0.0, 0.0), PxQuat(-PxHalfPi, PxVec3(0, 1, 0))));

			//finger_joint[i][0]->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

			//newHand.finger_joint[i][0]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
			newHand.finger_joint[i][0]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
			newHand.finger_joint[i][0]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
		}

	}
	gScene->addAggregate(*(newHand.aggregate));

	mHandList.push_back(newHand);

}

handActor* LeapClass::createRefHand(PxVec3 position)
{
	//Hand hand = calibrateHand;
	//if (!hand.isValid())
	//	return nullptr;
	handActor* newHand = new handActor;
	//newHand->isRef = true;
	//newHand->id = hand.id();
	//Leap::Matrix handTransform = hand.basis();
	//handTransform.origin = Leap::Vector(position.x, position.y, position.z);
	//handTransform = handTransform.rigidInverse();
	//PxVec3 col1 = PxVec3(hand.basis().xBasis.x, hand.basis().xBasis.y, hand.basis().xBasis.z);
	//PxVec3 col2 = PxVec3(hand.basis().yBasis.x, hand.basis().yBasis.y, hand.basis().yBasis.z);
	//PxVec3 col3 = PxVec3(hand.basis().zBasis.x, hand.basis().zBasis.y, hand.basis().zBasis.z);

	//PxMat33 flipmat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });
	//if (hand.isLeft())
	//{
	//	col1 = PxVec3(-hand.basis().xBasis.x, hand.basis().xBasis.y, hand.basis().xBasis.z);
	//	col2 = PxVec3(-hand.basis().yBasis.x, hand.basis().yBasis.y, hand.basis().yBasis.z);
	//	col3 = PxVec3(-hand.basis().zBasis.x, hand.basis().zBasis.y, hand.basis().zBasis.z);
	//	flipmat = PxMat33({ -1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });
	//}




	//PxQuat quat = PxQuat(-PxHalfPi,PxVec3(1,0,0));

	//PxMaterial* material = gPhysicsSDK->createMaterial(0.5f, 0.5f, 0.5f);
	//newHand->palm = PxCreateDynamic(*gPhysicsSDK, PxTransform(position, quat), PxBoxGeometry(0.3, 0.05, 0.4), *material, 1.0f);
	//if (newHand->palm == NULL)
	//{
	//	return nullptr;
	//}
	//newHand->palm->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
	//gScene->addActor(*(newHand->palm));
	//setupFiltering(newHand->palm, FilterGroup::ePARTICLE, FilterGroup::ePARTICLE);


	//int i = 0;
	//Leap::FingerList fingers = hand.fingers();
	//for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); fl++, i++)
	//{
	//	Leap::Bone bone;
	//	Leap::Bone::Type boneType;
	//	boneType = static_cast<Leap::Bone::Type>(3);
	//	bone = (*fl).bone(boneType);


	//	//newHand->finger_tip_achor[i] = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(bone.center())), PxSphereGeometry((*fl).width() / 100.0f), *material, 1.0f);
	//	//newHand->finger_tip_achor[i]->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
	//	//PxU32 nShapes = newHand->finger_tip_achor[i]->getNbShapes();
	//	//PxShape** shapes = new PxShape*[nShapes];
	//	//newHand->finger_tip_achor[i]->getShapes(shapes, nShapes);
	//	//while (nShapes--)
	//	//{
	//	//	shapes[nShapes]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	//	//}


	//	//newHand.finger_tip[i] = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(bone.center())), PxSphereGeometry((*fl).width() / 100.0f), *material, 1.0f);
	//	//newHand.finger_tip[i] = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(bone.center())), PxSphereGeometry((*fl).width() / 100.0f), *material, 1.0f);
	//	//newHand.finger_tip[i]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	//	//setupFiltering(newHand.finger_tip[i], FilterGroup::ePARTICLE, FilterGroup::ePARTICLE);
	//	////newHand.finger_tip_joint[i] = PxDistanceJointCreate(*gPhysicsSDK, newHand.finger_tip_achor[i], PxTransform(PxVec3(0, 0, 0)), newHand.finger_tip[i], PxTransform(PxVec3(0, 0, 0)));

	//	////newHand.finger_tip_joint[i]->setMaxDistance(0.01);
	//	////newHand.finger_tip_joint[i]->setStiffness(100.0f);
	//	////newHand.finger_tip_joint[i]->setDamping(10.0f);
	//	////newHand.finger_tip_joint[i]->setDistanceJointFlag(PxDistanceJointFlag::eSPRING_ENABLED, true);
	//	////newHand.finger_tip_joint[i]->setDistanceJointFlag(PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
	//	//gScene->addActor(*newHand.finger_tip[i]);
	//	//gScene->addActor(*newHand.finger_tip_achor[i]);



	//	for (int b = 1; b < 4; b++)
	//	{
	//		boneType = static_cast<Leap::Bone::Type>(b);
	//		bone = (*fl).bone(boneType);

	//		newHand->halfHeight[i][b - 1] = bone.length() / 200.0f;
	//		newHand->finger_actor[i][b - 1] = createCylinder(0.1, newHand->halfHeight[i][b - 1], leapToWorld(bone.center()),);
	//		newHand->finger_actor[i][b - 1]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	//		//newHand->finger_actor[i][b - 1]->setMass(2);
	//		setupFiltering(newHand->finger_actor[i][b - 1], FilterGroup::ePARTICLE, FilterGroup::ePARTICLE);
	//		//finger_actor[i][b - 1]->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);

	//		//proxyParticleActor.push_back(newHand->finger_actor[i][b - 1]);
	//		//achor[i][b - 1] = createJoint(bone.nextJoint(), finger_actor[i][b - 1], PxVec3(halfHeight[i][b - 1], 0, 0));
	//	}

	//	for (int j = 2; j >= 1; j--)
	//	{
	//		newHand->finger_joint[i][j] = PxD6JointCreate(*gPhysicsSDK, newHand->finger_actor[i][j - 1], PxTransform(PxVec3(-newHand->halfHeight[i][j - 1], 0.0, 0.0)), newHand->finger_actor[i][j], PxTransform(PxVec3(newHand->halfHeight[i][j], 0.0, 0.0)));
	//		newHand->finger_joint[i][j]->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
	//		newHand->finger_joint[i][j]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
	//		newHand->finger_joint[i][j]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
	//		newHand->finger_joint[i][j]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
	//	}


	//	boneType = static_cast<Leap::Bone::Type>(1);
	//	bone = (*fl).bone(boneType);
	//	Leap::Vector attachPos = handTransform.transformPoint(bone.prevJoint());
	//	PxVec3 attachPosMM = PxVec3(attachPos.x / 100.0, attachPos.y / 100.0, -attachPos.z / 100.0);
	//	if (hand.isLeft())
	//	{
	//		attachPosMM = PxVec3(-attachPos.x / 100.0, attachPos.y / 100.0, -attachPos.z / 100.0);
	//	}
	//	newHand->finger_joint[i][0] = PxD6JointCreate(*gPhysicsSDK, newHand->palm, PxTransform(attachPosMM), newHand->finger_actor[i][0], PxTransform(PxVec3(newHand->halfHeight[i][0], 0.0, 0.0), PxQuat(-PxHalfPi, PxVec3(0, 1, 0))));

	//	//finger_joint[i][0]->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
	//	newHand->finger_joint[i][0]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
	//	newHand->finger_joint[i][0]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
	//	newHand->finger_joint[i][0]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
	//}
	return newHand;

}

void LeapClass::updateHand(Hand hand, handActor actor)
{
	//if (!hands.isEmpty())
	//{
	//	int i = 0;
	//	Hand hand = hands[0];
	//	PxVec3 col1 = PxVec3(hand.basis().xBasis.x, hand.basis().xBasis.y, hand.basis().xBasis.z);
	//	PxVec3 col2 = PxVec3(hand.basis().yBasis.x, hand.basis().yBasis.y, hand.basis().yBasis.z);
	//	PxVec3 col3 = PxVec3(hand.basis().zBasis.x, hand.basis().zBasis.y, hand.basis().zBasis.z);

	//	PxMat33 flipmat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });

	//	PxQuat quat = PxQuat(flipmat*PxMat33(col1, col2, col3)*flipmat);
	//	PxTransform transform_update(leapToWorld(hand.palmPosition()), quat);
	//	//PxTransform transform_update(PxVec3(0.0, 0.0, 0.0));
	//	palm->setKinematicTarget(transform_update);

	//	i = 0;
	//	Leap::FingerList fingers = hand.fingers();
	//	for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); fl++, i++){
	//		Leap::Bone bone;
	//		Leap::Bone::Type boneType;
	//		for (int b = 1; b < 4; b++)
	//		{
	//			boneType = static_cast<Leap::Bone::Type>(b);
	//			bone = (*fl).bone(boneType);
	//			//Vector bonetip = bone.center() + bone.direction() * bone.length()*0.5f;
	//			PxTransform transform_update_joint(leapToWorld(bone.nextJoint()), PxQuat::createIdentity());

	//			achor[i][b - 1]->setKinematicTarget(transform_update_joint);

	//			PxVec3 v = achor[i][b - 1]->getLinearVelocity();
	//			finger_actor[i][b - 1]->setLinearVelocity(v);
	//		}
	//	}
	//}


		Leap::Matrix handTransform = hand.basis();
		handTransform.origin = hand.palmPosition();

		if (hand.isLeft())
		{
			handTransform.xBasis = -handTransform.xBasis;
		}


		handTransform = handTransform.rigidInverse();


		PxVec3 col1 = PxVec3(hand.basis().xBasis.x, hand.basis().xBasis.y, hand.basis().xBasis.z);
		PxVec3 col2 = PxVec3(hand.basis().yBasis.x, hand.basis().yBasis.y, hand.basis().yBasis.z);
		PxVec3 col3 = PxVec3(hand.basis().zBasis.x, hand.basis().zBasis.y, hand.basis().zBasis.z);
		PxMat33 flipmat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });
		if (hand.isLeft())
		{
			col1 = PxVec3(-hand.basis().xBasis.x, -hand.basis().xBasis.y, -hand.basis().xBasis.z);
			col2 = PxVec3(hand.basis().yBasis.x, hand.basis().yBasis.y, hand.basis().yBasis.z);
			col3 = PxVec3(hand.basis().zBasis.x, hand.basis().zBasis.y, hand.basis().zBasis.z);
		}
		PxMat33 rot = flipmat*PxMat33(col1, col2, col3)*flipmat;
		PxQuat quat = PxQuat(rot);
		PxTransform transform_update(leapToWorld(hand.palmPosition()), quat);

	if (!actor.isRef)
	{

		actor.palm->setKinematicTarget(transform_update);


	}

	PxD6JointDrive  drive1(100.0, 0.0, PX_MAX_F32, false);
	PxD6JointDrive  drive2(1000.0, 0.0, PX_MAX_F32, true);
	PxD6JointDrive  drive3(500.0, 0.0f, PX_MAX_F32, false);


	int i = 0;
	actor.grabStrength = hand.grabStrength();
	actor.pinchStrength = hand.pinchStrength();

	Leap::FingerList fingers = hand.fingers();
	for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); fl++, i++)
	{
		Leap::Bone bone;
		Leap::Bone::Type boneType;

		Leap::Matrix localFingerBasis;
		PxMat33 rotatemat;
		Leap::Matrix preBone = handTransform;
		if (i == 0)
		{
			
			actor.finger_joint[i][1]->setDrive(PxD6Drive::eSWING, drive2);
			//actor.finger_joint[i][1]->setDrive(PxD6Drive::eTWIST, drive2);
			boneType = static_cast<Leap::Bone::Type>(1);
			bone = (*fl).bone(boneType);
			Leap::Matrix bonebasis = bone.basis();

			if (hand.isLeft())
				bonebasis.xBasis = -bonebasis.xBasis;

			localFingerBasis = preBone*bonebasis;

			PxVec3 col1 = PxVec3(localFingerBasis.xBasis.x, localFingerBasis.xBasis.y, localFingerBasis.xBasis.z);
			PxVec3 col2 = PxVec3(localFingerBasis.yBasis.x, localFingerBasis.yBasis.y, localFingerBasis.yBasis.z);
			PxVec3 col3 = PxVec3(localFingerBasis.zBasis.x, localFingerBasis.zBasis.y, localFingerBasis.zBasis.z);
			rotatemat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });
			PxQuat quat = PxQuat(rotatemat*PxMat33(col1, col2, col3)*rotatemat);


			//finger_actor[i][0]->setGlobalPose(PxTransform(leapToWorld(bone.prevJoint()), quat));
			actor.finger_joint[i][1]->setDrivePosition(PxTransform(quat));
			preBone = bonebasis.rigidInverse();

			for (int j = 2; j < 4; j++)
			{
				//actor.finger_joint[i][j]->setDrive(PxD6Drive::eSLERP, drive2);
				//actor.finger_joint[i][j]->setBaseFlag(PxBaseFlag::)
				actor.finger_joint[i][j]->setDrive(PxD6Drive::eSWING, drive2);
				//actor.finger_joint[i][j]->setDrive(PxD6Drive::eTWIST, drive2);
				boneType = static_cast<Leap::Bone::Type>(j);
				bone = (*fl).bone(boneType);
				Leap::Matrix bonebasis = bone.basis();

				if (hand.isLeft())
					bonebasis.xBasis = -bonebasis.xBasis;

				localFingerBasis = preBone*bonebasis;

				PxVec3 col1 = PxVec3(localFingerBasis.xBasis.x, localFingerBasis.xBasis.y, localFingerBasis.xBasis.z);
				PxVec3 col2 = PxVec3(localFingerBasis.yBasis.x, localFingerBasis.yBasis.y, localFingerBasis.yBasis.z);
				PxVec3 col3 = PxVec3(localFingerBasis.zBasis.x, localFingerBasis.zBasis.y, localFingerBasis.zBasis.z);
				rotatemat = PxMat33({ 0, 0, 1 }, { 0, 1, 0 }, { 1, 0, 0 });
				PxQuat quat = PxQuat(rotatemat*PxMat33(col1, col2, col3)*rotatemat);


				//finger_actor[i][0]->setGlobalPose(PxTransform(leapToWorld(bone.prevJoint()), quat));
				actor.finger_joint[i][j]->setDrivePosition(PxTransform(quat));
				preBone = bonebasis.rigidInverse();
			}
		}
		else
		{

			actor.finger_joint[i][0]->setDrive(PxD6Drive::eSWING, drive2);
			boneType = static_cast<Leap::Bone::Type>(0);
			bone = (*fl).bone(boneType);
			Leap::Matrix bonebasis = bone.basis();

			if (hand.isLeft())
				bonebasis.xBasis = -bonebasis.xBasis;

			localFingerBasis = preBone*bonebasis;

			PxVec3 col1 = PxVec3(localFingerBasis.xBasis.x, localFingerBasis.xBasis.y, localFingerBasis.xBasis.z);
			PxVec3 col2 = PxVec3(localFingerBasis.yBasis.x, localFingerBasis.yBasis.y, localFingerBasis.yBasis.z);
			PxVec3 col3 = PxVec3(localFingerBasis.zBasis.x, localFingerBasis.zBasis.y, localFingerBasis.zBasis.z);
			rotatemat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });
			PxQuat quat = PxQuat(rotatemat*PxMat33(col1, col2, col3)*rotatemat);


			//finger_actor[i][0]->setGlobalPose(PxTransform(leapToWorld(bone.prevJoint()), quat));
			actor.finger_joint[i][0]->setDrivePosition(PxTransform(quat));
			preBone = bonebasis.rigidInverse();

			for (int j = 1; j < 4; j++)
			{
				actor.finger_joint[i][j]->setDrive(PxD6Drive::eSWING, drive2);
				boneType = static_cast<Leap::Bone::Type>(j);
				bone = (*fl).bone(boneType);
				Leap::Matrix bonebasis = bone.basis();

				if (hand.isLeft())
					bonebasis.xBasis = -bonebasis.xBasis;

				localFingerBasis = preBone*bonebasis;

				PxVec3 col1 = PxVec3(localFingerBasis.xBasis.x, localFingerBasis.xBasis.y, localFingerBasis.xBasis.z);
				PxVec3 col2 = PxVec3(localFingerBasis.yBasis.x, localFingerBasis.yBasis.y, localFingerBasis.yBasis.z);
				PxVec3 col3 = PxVec3(localFingerBasis.zBasis.x, localFingerBasis.zBasis.y, localFingerBasis.zBasis.z);
				rotatemat = PxMat33({ 0, 0, 1 }, { 0, 1, 0 }, { 1, 0, 0 });
				PxQuat quat = PxQuat(rotatemat*PxMat33(col1, col2, col3)*rotatemat);


				//finger_actor[i][0]->setGlobalPose(PxTransform(leapToWorld(bone.prevJoint()), quat));
				actor.finger_joint[i][j]->setDrivePosition(PxTransform(quat));
				preBone = bonebasis.rigidInverse();
			}
		}
	}


}

void LeapClass::deleteHand(handActor actor)
{
	actor.palm->release();
	actor.palm = NULL;
	for (int i = 0; i < 5; i++)
	{
		int j = 0;
		if(i==0)
		 j = 1;


		for (; j < 4; j++)
		{
			////achor[i][j]->release();
			gScene->removeActor(*actor.finger_actor[i][j]);
			actor.finger_actor[i][j]->release();
			actor.finger_joint[i][j]->release();
		}
	}
	gScene->removeAggregate(*(actor.aggregate));
	actor.aggregate->release();
}

PxRigidDynamic* LeapClass::createJoint(Vector bone, PxRigidDynamic* finger_joint, PxVec3 attachPosition)
{
	float maxDist = 0.05f;
	float damping = 10.0;
	float stifness = 10.0f;
	PxRigidDynamic* newActor = CreateSphere(leapToWorld(bone), 0.1, 1.0f);
	newActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);

	PxU32 nShapes = newActor->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];
	newActor->getShapes(shapes, nShapes);

	while (nShapes--)
	{
		shapes[nShapes]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	}
	PxDistanceJoint* joint = PxDistanceJointCreate(*gPhysicsSDK, newActor, PxTransform(PxVec3(0.0, 0.0, 0.0)), finger_joint, PxTransform(attachPosition));
	joint->setMaxDistance(maxDist);
	joint->setDamping(damping);
	joint->setStiffness(stifness);
	joint->setDistanceJointFlag(PxDistanceJointFlag::eSPRING_ENABLED, true);
	joint->setDistanceJointFlag(PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);

	return newActor;
}

PxRigidDynamic* LeapClass::createCylinder(PxReal radius, PxReal halfHeight, PxVec3 pos, PxAggregate* aggregate)
{
	if (halfHeight == 0)
		halfHeight = 0.1;
	PxTransform transform(pos, PxQuat(PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f)));
	PxMaterial* mMaterial = gPhysicsSDK->createMaterial(1.0, 1.0, 1.0);
	PxReal         density = 1.0f;
	PxCapsuleGeometry geometry(radius, halfHeight);
	PxRigidDynamic *actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);


//	actor->setLinearVelocity(PxVec3(0, 0, 0));
	actor->setSolverIterationCounts(16, 8);
	if (!actor)
		cerr << "create actor failed!" << endl;
	aggregate->addActor(*actor);

	return actor;
}

std::vector<handActor> LeapClass::getHandActor()
{
	return mHandList;
}

void LeapClass::clearHand()
{
	for (int i = 0; i < mHandList.size(); i++)
		deleteHand(mHandList[i]);

	mHandList.clear();
}

std::map<int, PxVec3> LeapClass::computeForce(std::map<int, PxRigidDynamic*> activeContact)
{
	std::map<int, PxVec3> forces;
	for (map<int , PxRigidDynamic*>::iterator ii = activeContact.begin(); ii != activeContact.end(); ++ii)
	{
		Frame frame = controller.frame();
		Hand hand = frame.hand((*ii).first);
		if (!hand.isValid() || (*ii).second == nullptr)
			forces[(*ii).first] = PxVec3(0, 0, 0);
		else
		{
			if (hand.grabStrength() > 0.6 || hand.pinchStrength() > 0.6)
			{
				PxVec3 handPos = leapToWorld(hand.palmPosition());
				PxVec3 actorPos = (*ii).second->getGlobalPose().p;
				PxVec3 d = handPos - actorPos;
				Leap::Vector n = hand.basis().yBasis;
				PxVec3 pxn = PxVec3(n.x, n.y, n.z);
				PxVec3 dn = d.getNormalized();

				if (dn.dot(pxn) > 0)
					forces[(*ii).first] = dn*0.05;
				else
					forces[(*ii).first] = (PxVec3(0, 0, 0));
			}
			else
			{
				forces[(*ii).first] = (PxVec3(0, 0, 0));
			}
		}

	}
	return forces;

}