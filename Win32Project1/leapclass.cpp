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
		eTarget = ( 1 << 3),
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

void LeapClass::processFrame(float headPosition_x, float headPosition_y, float headPosition_z, float offset_z, XMFLOAT4X4 mView, XMFLOAT4X4 mProj, float handlength)
{
	while (!controller.isConnected());
	Frame frame = controller.frame();
	if (frame.id() == lastFrameID) return;

	x_head = headPosition_x;
	y_head = headPosition_y;
	z_head = headPosition_z;

	HandList hands = frame.hands();
	//bool createHand = false;

	z_offset = offset_z;
	viewMatrix = mView;
	projectionMatrix = mProj;
	
	//FingerList fingers = hand.fingers();
	//Leap::PointableList pointables = frame.pointables();
	//Leap::FingerList fingers = frame.fingers();
	//Leap::ToolList tools = frame.tools();


	if (!hands.isEmpty() && mHandList.size() == 0)
	{
		for (int i = 0; i < hands.count(); i++)
		{
			if(hands[i].isValid() && hands[i].palmPosition().y > 100.0f)
				createHand(hands[i], handlength);
		}
		

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
			std::vector<int> tobeDelete;
			for (int i = 0; i < mHandList.size(); i++)
			{
				if(frame.hand(mHandList[i].id).isValid() && frame.hand(mHandList[i].id).palmPosition().y > 100.0f)
					updateHand(frame.hand(mHandList[i].id), mHandList[i]);
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
					if (hands[i].isValid() && hands[i].palmPosition().y > 100.0f)
						createHand(hands[i], handlength);
				}
			}
		}
		else if (hands_count < (int)mHandList.size())
		{
			std::vector<int> tobeDelete;
			for (int i = 0; i < mHandList.size(); i++)
			{
				if (frame.hand(mHandList[i].id).isValid() && frame.hand(mHandList[i].id).palmPosition().y > 100.0f)
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
		{
			std::vector<int> tobeDelete;
			for (int i = 0; i < mHandList.size(); i++)
			{
				if (frame.hand(mHandList[i].id).isValid() && frame.hand(mHandList[i].id).palmPosition().y > 100.0f)
					updateHand(frame.hand(mHandList[i].id), mHandList[i]);
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


	lastFrameID = frame.id();
}

PxRigidDynamic* LeapClass::CreateSphere(const PxVec3& pos, const PxReal radius, const PxReal density)
{
	PxTransform transform(pos, PxQuat::createIdentity());
	PxSphereGeometry geometry(radius);

	PxMaterial* mMaterial = gPhysicsSDK->createMaterial(1.0, 1.0, 1.0);

	PxRigidDynamic* actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);

	if (!actor)
		cerr << "create actor failed" << endl;
	//actor->setAngularDamping(0.75);
	actor->setLinearVelocity(PxVec3(0, 0, 0));
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
	converted.y = bone_center.y / 100.f - 2.7;
	converted.z = -bone_center.z / 100.f - 2.6f;
	float normalized_y = y_head - 1.7;

	converted.x = (x_head-converted.x)*z_head/ (z_head - converted.z);
	converted.y = (normalized_y -converted.y)*z_head / (z_head - converted.z);

	float sx = -(converted.x - x_head)/ 3.0f;
	float sy  = -(converted.y- normalized_y) /1.7f;

	float xScale = projectionMatrix._11;
	float yScale = projectionMatrix._22;
	float xx = viewMatrix._11;
	float xy = viewMatrix._21;
	float xz = viewMatrix._31;
	float dx = -viewMatrix._41;

	float yx = viewMatrix._12;
	float yy = viewMatrix._22;
	float yz = viewMatrix._32;
	float dy = -viewMatrix._42;

	float zx = viewMatrix._13;
	float zy = viewMatrix._23;
	float zz = viewMatrix._33;
	float dz = -viewMatrix._43;

	float rl = projectionMatrix._31;
	float tb = projectionMatrix._32;
	float zt = projectionMatrix._33;
	float zb = projectionMatrix._43;

	float xp, yp, zp = converted.z + z_offset;


	MatrixXf a(2,2);
	VectorXf b(2);

	if (!CORRECTPERS)
	{
		a(0, 0) = -xScale*xx + sx*zx;
		a(1, 0) = -yScale*yx + sy*zx;
		a(0, 1) = -xScale*xy + sx*zy;
		a(1, 1) = -yScale*yy + sy*zy;

		b(0) = dz*sx - dx*xScale + xScale*xz*zp - sx*zp*zz;
		b(1) = dz*sy - dy*yScale + yScale*yz*zp - sy*zp*zz;
	}
	else
	{
		a(0, 0) = -xScale*xx -rl *zx+sx*zx;
		a(1, 0) = -yScale*yx +sy*zx-tb*zx;
		a(0, 1) = -xScale*xy-rl*zy+sx*zy;
		a(1, 1) = -yScale*yy+sy*zy-tb*zy;

		b(0) = -dz*rl + dz*sx - dx*xScale + xScale*xz*zp + rl*zp*zz - sx*zp*zz;
		b(1) = dz*sy - dz*tb - dy*yScale + yScale*yz*zp - sy*zp*zz + tb*zp*zz;
	}

	VectorXf c = a.jacobiSvd(ComputeThinU | ComputeThinV).solve(b);
	//converted.z = (converted.z - 0.1) / (-0.1*converted.z);

	converted.x = c(0);
	converted.y = c(1);
	converted.z = zp;





	//converted.x = xs*(converted.z+z_offset + 8.0f) / x_scale;
	//converted.y = (ys*(converted.z+ z_offset + 8.0f) + y_scale*1.5) / y_scale;
	//converted.z = converted.z + z_offset;

	//converted.w = 1;

	//PxVec4 buffer = projectionMatrix.transform(converted);
	//PxVec4 result = viewMatrix.transform(buffer);


	//PxVec3 result;
	//result = ltoworldtransform.transform(converted);
	//

	return converted;//PxVec3(result.x/result.w,result.y/result.w,result.z/result.w);

}
//
//PxVec3 LeapClass::leapToWorld(Leap::Vector bone_center)
//{
//	PxVec3 converted;
//	converted.x = bone_center.x / 100.f;
//	converted.y = bone_center.y / 100.f - 2.7;
//	converted.z = -bone_center.z / 100.f - 2.6f;
//	float normalized_y = y_head - 1.7;
//
//	converted.x = (x_head - converted.x)*z_head / (z_head - converted.z);
//	converted.y = (normalized_y - converted.y)*z_head / (z_head - converted.z);
//
//	float sx = -(converted.x - x_head) / 3.0f;
//	float sy = -(converted.y - normalized_y) / 1.7f;
//
//	float xScale = projectionMatrix._11;
//	float yScale = projectionMatrix._22;
//	float xx = viewMatrix._11;
//	float xy = viewMatrix._21;
//	float xz = viewMatrix._31;
//	float dx = -viewMatrix._41;
//
//	float yx = viewMatrix._12;
//	float yy = viewMatrix._22;
//	float yz = viewMatrix._32;
//	float dy = -viewMatrix._42;
//
//	float zx = viewMatrix._13;
//	float zy = viewMatrix._23;
//	float zz = viewMatrix._33;
//	float dz = -viewMatrix._43;
//
//	float xp, yp, zp = converted.z + z_offset;
//	MatrixXf a(2, 2);
//	a(0, 0) = -xScale*xx + sx*zx;
//	a(1, 0) = -yScale*yx + sy*zx;
//	a(0, 1) = -xScale*xy + sx*zy;
//	a(1, 1) = -yScale*yy + sy*zy;
//	VectorXf b(2);
//	b(0) = dz*sx - dx*xScale + xScale*xz*zp - sx*zp*zz;
//	b(1) = dz*sy - dy*yScale + yScale*yz*zp - sy*zp*zz;
//	VectorXf c = a.jacobiSvd(ComputeThinU | ComputeThinV).solve(b);
//	//converted.z = (converted.z - 0.1) / (-0.1*converted.z);
//
//	converted.x = c(0);
//	converted.y = c(1);
//	converted.z = zp;
//
//
//
//
//
//	//converted.x = xs*(converted.z+z_offset + 8.0f) / x_scale;
//	//converted.y = (ys*(converted.z+ z_offset + 8.0f) + y_scale*1.5) / y_scale;
//	//converted.z = converted.z + z_offset;
//
//	//converted.w = 1;
//
//	//PxVec4 buffer = projectionMatrix.transform(converted);
//	//PxVec4 result = viewMatrix.transform(buffer);
//
//
//	//PxVec3 result;
//	//result = ltoworldtransform.transform(converted);
//	//
//
//	return converted;//PxVec3(result.x/result.w,result.y/result.w,result.z/result.w);
//
//}


void LeapClass::setProjectionMatrix(PxMat44 mat)
{
	//projectionMatrix = mat;
}

void LeapClass::setViewMatrix(PxMat44 mat)
{
	//viewMatrix = mat;
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

void LeapClass::createHand(Hand hand, float handLength)
{



	//Create Palm
	float damping = 1.0f;
	float stifness = 10000.0f;
	float maxDist = 0.001f;
	PxU32 nbActors = 45;

	float mass[4] = {0.1, 0.1,0.075,0.05 };
	PxVec3 I_tensor = PxVec3(0.7,0.7,0.7);
	bool selfCollision = false;
	handActor newHand;

	newHand.aggregate = gPhysicsSDK->createAggregate(nbActors, selfCollision);
	newHand.isRef = false;
	newHand.id = hand.id();
	Leap::Matrix handTransform = hand.basis();
	handTransform.origin = hand.palmPosition();
	

	if (hand.isLeft())
	{
		handTransform.xBasis = -handTransform.xBasis;
	}

	handTransform = handTransform.rigidInverse();

	PxVec3 col1, col2, col3;
	PxMat33 flipmat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });
	if (hand.isLeft())
	{
		col1 = PxVec3(-hand.basis().xBasis.x, -hand.basis().xBasis.y, -hand.basis().xBasis.z);
		col2 = PxVec3(hand.basis().yBasis.x, hand.basis().yBasis.y, hand.basis().yBasis.z);
		col3 = PxVec3(hand.basis().zBasis.x, hand.basis().zBasis.y, hand.basis().zBasis.z);
	}
	else
	{

		col1 = PxVec3(hand.basis().xBasis.x, hand.basis().xBasis.y, hand.basis().xBasis.z);
		col2 = PxVec3(hand.basis().yBasis.x, hand.basis().yBasis.y, hand.basis().yBasis.z);
		col3 = PxVec3(hand.basis().zBasis.x, hand.basis().zBasis.y, hand.basis().zBasis.z);
	}


	PxQuat quat = PxQuat(flipmat*PxMat33(col1, col2, col3)*flipmat);

	PxMaterial* material = gPhysicsSDK->createMaterial(1.0f, 1.0f, 1.0f);
	newHand.palm = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(hand.palmPosition()),quat), PxBoxGeometry((hand.palmWidth() - hand.fingers()[0].width())/200, 0.1, hand.palmWidth() / 200 ), *material, 1.0f);
	if (newHand.palm == NULL)
	{
		return;
	}
	newHand.palm->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
	PxShape* shapes;
	newHand.palm->getShapes(&shapes, 1);
	shapes->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	shapes->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);


	setupFiltering(newHand.palm, FilterGroup::eHand, FilterGroup::eBox | FilterGroup::eTarget);

	newHand.aggregate->addActor(*newHand.palm);

	Leap::Finger middle = hand.fingers()[2];
	Leap::Bone bone;
	Leap::Bone::Type boneType;
	float middle_length = 0;
	for (int b = 0; b < 4; b++)
	{
		boneType = static_cast<Leap::Bone::Type>(b);
		bone = (middle).bone(boneType);
		middle_length += bone.length();
	}

	float factor = 1;// handLength / middle_length;


	newHand.palmDimension = PxVec3(hand.palmWidth()/120.0f, hand.palmWidth()/100.0f, hand.palmWidth()/400.0f);
	newHand.leapJointPosition = new PxVec3[20];
	newHand.leapJointPosition2 = new PxVec3[20];
	newHand.fingerTipPosition = new PxVec3[5];
	newHand.isExtended = new bool[5];

	int i = 0;
	Leap::FingerList fingers = hand.fingers();
	for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); fl++, i++)
	{
		Leap::Bone bone;
		Leap::Bone::Type boneType;

		Leap::Matrix localFingerBasis;
		PxMat33 rotatemat;
		Leap::Matrix preBone = handTransform;

		//newHand.finger_tip_achor[i] = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld((*fl).tipPosition())), PxSphereGeometry((*fl).width() / 200.0f), *material, 1.0f);
		//newHand.finger_tip_achor[i]->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
		//PxShape* shapes;
		//newHand.finger_tip_achor[i]->getShapes(&shapes, 1);
		//shapes->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);

		// newHand.aggregate->addActor(*newHand.finger_tip_achor[i]);
		// Leap::Vector attachpos_prev, attachpos_cur;
		 

		if (i == 0)
		{
			boneType = static_cast<Leap::Bone::Type>(1);
			bone = (*fl).bone(boneType);

			newHand.halfHeight[i][1] = bone.length() / 200.0f;
			newHand.fingerWidth[i][1] = factor*bone.width() / 100.0f;
			newHand.finger_actor[i][1] = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(bone.center())),
				PxBoxGeometry(newHand.halfHeight[i][1], newHand.fingerWidth[i][1] / 4.0f, newHand.fingerWidth[i][1] / 2.0f), *material, 1.0f);

			newHand.finger_actor[i][1]->setMass(mass[1]);
			newHand.finger_actor[i][1]->setMassSpaceInertiaTensor(I_tensor);

			newHand.finger_actor[i][1]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			setupFiltering(newHand.finger_actor[i][1], FilterGroup::eFinger, FilterGroup::eBox | FilterGroup::eTarget);
			newHand.aggregate->addActor(*newHand.finger_actor[i][1]);

			newHand.finger_joint_actor[i][1] = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(bone.center())), PxSphereGeometry((*fl).width() / 200.0f), *material, 1.0f);
			newHand.finger_joint_actor[i][1]->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
			PxShape* shapes;
			newHand.finger_joint_actor[i][1]->getShapes(&shapes, 1);
			shapes->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);

			newHand.aggregate->addActor(*newHand.finger_joint_actor[i][1]);

			newHand.finger_joint[i][1] = PxD6JointCreate(*gPhysicsSDK, newHand.finger_joint_actor[i][1], PxTransform(PxVec3(0, 0, 0)), newHand.finger_actor[i][1], PxTransform(PxVec3(0, 0, 0), PxQuat(PxHalfPi, PxVec3(0, 1, 0))));
			//newHand.finger_joint[i][0]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
			newHand.finger_joint[i][1]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
			newHand.finger_joint[i][1]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
			newHand.finger_joint[i][1]->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
			newHand.finger_joint[i][1]->setMotion(PxD6Axis::eY, PxD6Motion::eFREE);
			newHand.finger_joint[i][1]->setMotion(PxD6Axis::eZ, PxD6Motion::eFREE);
			//newHand.finger_joint[i][1]->setLinearLimit(PxJointLinearLimit(maxDist,PxSpring(stifness,damping)));
			
			for (int j = 2; j < 4; j++)
			{
				boneType = static_cast<Leap::Bone::Type>(j);
				bone = (*fl).bone(boneType);

				newHand.halfHeight[i][j] = bone.length() / 200.0f;
				newHand.fingerWidth[i][j] = factor*bone.width() / 100.0f;
				newHand.finger_actor[i][j] = createCylinder(bone.width() / 200.0f, newHand.halfHeight[i][j], leapToWorld(bone.center()), PxQuat::createIdentity(), newHand.aggregate);
				newHand.finger_actor[i][j]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
				newHand.finger_actor[i][j]->setMass(mass[j]);
				newHand.finger_actor[i][j]->setMassSpaceInertiaTensor(I_tensor);
				setupFiltering(newHand.finger_actor[i][j], FilterGroup::eFinger, FilterGroup::eBox | FilterGroup::eTarget);

				newHand.aggregate->addActor(*newHand.finger_actor[i][j]);

				newHand.finger_joint_actor[i][j] = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(bone.center())), PxSphereGeometry((*fl).width() / 200.0f), *material, 1.0f);
				newHand.finger_joint_actor[i][j]->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
				PxShape* shapes;
				newHand.finger_joint_actor[i][j]->getShapes(&shapes, 1);
				shapes->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);

				newHand.aggregate->addActor(*newHand.finger_joint_actor[i][j]);

				newHand.leapJointPosition[i * 4 + j] = PxVec3(bone.center().x, bone.center().y, bone.center().z);

				newHand.finger_joint[i][j] = PxD6JointCreate(*gPhysicsSDK, newHand.finger_joint_actor[i][j], PxTransform(PxVec3(0, 0, 0)), newHand.finger_actor[i][j], PxTransform(PxVec3(0, 0, 0), PxQuat(PxHalfPi, PxVec3(0, 1, 0))));
				//newHand.finger_joint[i][j]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);

				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eY, PxD6Motion::eFREE);
				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eZ, PxD6Motion::eFREE);
				//newHand.finger_joint[i][j]->setLinearLimit(PxJointLinearLimit(maxDist, PxSpring(stifness, damping)));
			}
		}
		else
		{
			
			boneType = static_cast<Leap::Bone::Type>(0);
			bone = (*fl).bone(boneType);

			newHand.halfHeight[i][0] = bone.length()/ 200.0f;
			newHand.fingerWidth[i][0] = factor*bone.width() / 100.0f;
			newHand.finger_actor[i][0] = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(bone.center())),
				PxBoxGeometry(newHand.halfHeight[i][0], newHand.fingerWidth[i][0] / 4.0f, newHand.fingerWidth[i][0] / 2.0f), *material, 1.0f);

			newHand.finger_actor[i][0]->setMass(mass[0]);
			newHand.finger_actor[i][0]->setMassSpaceInertiaTensor(I_tensor);

			newHand.finger_actor[i][0]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			setupFiltering(newHand.finger_actor[i][0], FilterGroup::eFinger, FilterGroup::eBox | FilterGroup::eTarget);
			newHand.aggregate->addActor(*newHand.finger_actor[i][0]);

			newHand.finger_joint_actor[i][0] = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(bone.center())), PxSphereGeometry((*fl).width() / 200.0f), *material, 1.0f);
			newHand.finger_joint_actor[i][0]->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
			PxShape* shapes;
			newHand.finger_joint_actor[i][0]->getShapes(&shapes, 1);
			shapes->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);

			newHand.aggregate->addActor(*newHand.finger_joint_actor[i][0]);

			newHand.finger_joint[i][0] = PxD6JointCreate(*gPhysicsSDK, newHand.finger_joint_actor[i][0], PxTransform(PxVec3(0,0,0)), newHand.finger_actor[i][0], PxTransform(PxVec3(0,0,0),PxQuat(PxHalfPi,PxVec3(0,1,0))));
			//newHand.finger_joint[i][0]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
			newHand.finger_joint[i][0]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
			newHand.finger_joint[i][0]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);

			newHand.finger_joint[i][0]->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
			newHand.finger_joint[i][0]->setMotion(PxD6Axis::eY, PxD6Motion::eFREE);
			newHand.finger_joint[i][0]->setMotion(PxD6Axis::eZ, PxD6Motion::eFREE);
			//newHand.finger_joint[i][0]->setLinearLimit(PxJointLinearLimit(maxDist, PxSpring(stifness, damping)));

			for (int j = 1; j < 4; j++)
			{
				boneType = static_cast<Leap::Bone::Type>(j);
				bone = (*fl).bone(boneType);

				newHand.halfHeight[i][j] = bone.length() / 200.0f;
				newHand.fingerWidth[i][j] = factor*bone.width() / 100.0f;
				newHand.finger_actor[i][j] = createCylinder(bone.width() / 200.0f, newHand.halfHeight[i][j], leapToWorld(bone.center()),PxQuat::createIdentity(), newHand.aggregate);
				newHand.finger_actor[i][j]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
				newHand.finger_actor[i][j]->setMass(mass[j]);
				newHand.finger_actor[i][j]->setMassSpaceInertiaTensor(I_tensor);
				setupFiltering(newHand.finger_actor[i][j], FilterGroup::eFinger, FilterGroup::eBox | FilterGroup::eTarget);

				newHand.aggregate->addActor(*newHand.finger_actor[i][j]);

				newHand.finger_joint_actor[i][j] = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(bone.center())), PxSphereGeometry((*fl).width() / 200.0f), *material, 1.0f);
				newHand.finger_joint_actor[i][j]->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
				PxShape* shapes;
				newHand.finger_joint_actor[i][j]->getShapes(&shapes, 1);
				shapes->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);

				newHand.aggregate->addActor(*newHand.finger_joint_actor[i][j]);

				newHand.leapJointPosition[i * 4 + j] = PxVec3(bone.center().x, bone.center().y, bone.center().z);

				newHand.finger_joint[i][j] = PxD6JointCreate(*gPhysicsSDK, newHand.finger_joint_actor[i][j], PxTransform(PxVec3(0, 0, 0)), newHand.finger_actor[i][j], PxTransform(PxVec3(0,0,0),PxQuat(PxHalfPi, PxVec3(0,1,0))));
				//newHand.finger_joint[i][j]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);

				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eY, PxD6Motion::eFREE);
				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eZ, PxD6Motion::eFREE);
				//newHand.finger_joint[i][j]->setLinearLimit(PxJointLinearLimit(maxDist, PxSpring(stifness, damping)));
			}
		}
		//newHand.finger_tip_joint[i] = PxDistanceJointCreate(*gPhysicsSDK, newHand.finger_tip_achor[i], PxTransform(PxVec3(0, 0, 0)), newHand.finger_actor[i][3], PxTransform(PxVec3(-newHand.halfHeight[i][3], 0.0, 0.0)));
		//newHand.finger_tip_joint[i]->setMaxDistance(0.05f);
		//newHand.finger_tip_joint[i]->setStiffness(1000.0f);
		//newHand.finger_tip_joint[i]->setDamping(0.05f);
		//newHand.finger_tip_joint[i]->setDistanceJointFlag(PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
		//newHand.finger_tip_joint[i]->setDistanceJointFlag(PxDistanceJointFlag::eSPRING_ENABLED, true);
	
	}

	/*
	for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); fl++, i++)
	{
		Leap::Bone bone;
		Leap::Bone::Type boneType;
		Leap::Vector tip = (*fl).tipPosition();
		newHand.fingerTipPosition[i] = PxVec3(tip.x, tip.y, tip.z);

		newHand.isExtended[i] = (*fl).isExtended();

		if (i == 0) // threat thumb different
		{
			for (int b = 2; b < 4; b++)
			{
				boneType = static_cast<Leap::Bone::Type>(b);
				bone = (*fl).bone(boneType);

				Leap::Matrix bonebasis = bone.basis();

				
				newHand.halfHeight[i][b] = factor* bone.length()/ 200.0f;
				newHand.fingerWidth[i][b] = factor*bone.width() / 100.0f;

				newHand.finger_actor[i][b] = createCylinder((*fl).width() / 250.0f, newHand.halfHeight[i][b], leapToWorld(bone.center()), newHand.aggregate);
				newHand.finger_actor[i][b]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
				newHand.finger_actor[i][b]->setMass(mass[b-1]);
				newHand.finger_actor[i][b]->setMassSpaceInertiaTensor(I_tensor);
				setupFiltering(newHand.finger_actor[i][b], FilterGroup::eFinger, FilterGroup::eBox| FilterGroup::eTarget);

				newHand.leapJointPosition[b] = PxVec3(bone.center().x, bone.center().y, bone.center().z);
			}

			boneType = static_cast<Leap::Bone::Type>(1);
			bone = (*fl).bone(boneType);

			newHand.halfHeight[i][1] = factor*bone.length() / 200.0f;
			newHand.fingerWidth[i][1] = factor*bone.width() / 100.0f;
			newHand.finger_actor[i][1] = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(bone.center())),
				PxBoxGeometry(newHand.halfHeight[i][1], newHand.fingerWidth[i][1] / 4.0f, newHand.fingerWidth[i][1] / 2.0f), *material, 1.0f);

			newHand.finger_actor[i][1]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			newHand.finger_actor[i][1]->setMass(mass[0]);
			newHand.finger_actor[i][1]->setMassSpaceInertiaTensor(I_tensor);
			setupFiltering(newHand.finger_actor[i][1], FilterGroup::eFinger, FilterGroup::eBox| FilterGroup::eTarget);
			newHand.aggregate->addActor(*newHand.finger_actor[i][1]);

			newHand.leapJointPosition[0] = PxVec3((*fl).tipPosition().x, (*fl).tipPosition().y, (*fl).tipPosition().z);
			newHand.leapJointPosition[1] = PxVec3(bone.center().x, bone.center().y, bone.center().z);

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
			newHand.finger_joint[i][1] = PxD6JointCreate(*gPhysicsSDK, newHand.palm, PxTransform(attachPosMM), newHand.finger_actor[i][1], PxTransform(PxVec3(newHand.halfHeight[i][1], newHand.fingerWidth[i][1] / 2.0f, 0.0), PxQuat(-PxHalfPi, PxVec3(0, 1, 0))));

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

				newHand.finger_actor[i][b] = createCylinder(bone.width() / 200.0f, newHand.halfHeight[i][b], leapToWorld(bone.center()), newHand.aggregate);
				newHand.finger_actor[i][b]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
				newHand.finger_actor[i][b]->setMass(mass[b]);
				newHand.finger_actor[i][b]->setMassSpaceInertiaTensor(I_tensor);

				setupFiltering(newHand.finger_actor[i][b], FilterGroup::eFinger, FilterGroup::eBox| FilterGroup::eTarget);

				newHand.leapJointPosition[i*4+b] = PxVec3(bone.center().x, bone.center().y, bone.center().z);

			}

			boneType = static_cast<Leap::Bone::Type>(0);
			bone = (*fl).bone(boneType);

			newHand.halfHeight[i][0] = factor*bone.length() / 200.0f;
			newHand.fingerWidth[i][0] = factor*bone.width() / 100.0f;
			newHand.finger_actor[i][0] = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(bone.center())),
				PxBoxGeometry(newHand.halfHeight[i][0], newHand.fingerWidth[i][0] / 4.0f, newHand.fingerWidth[i][0] / 2.0f), *material, 1.0f);

			newHand.finger_actor[i][0]->setMass(mass[0]);
			newHand.finger_actor[i][0]->setMassSpaceInertiaTensor(I_tensor);


			newHand.finger_actor[i][0]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			setupFiltering(newHand.finger_actor[i][0], FilterGroup::eFinger, FilterGroup::eBox| FilterGroup::eTarget);
			newHand.aggregate->addActor(*newHand.finger_actor[i][0]);

			newHand.leapJointPosition[i * 4] = PxVec3(bone.center().x, bone.center().y, bone.center().z);

			for (int j = 3; j >= 2; j--)
			{

				newHand.finger_joint[i][j] = PxD6JointCreate(*gPhysicsSDK, newHand.finger_actor[i][j - 1], PxTransform(PxVec3(-newHand.halfHeight[i][j - 1], 0.0, 0.0)), newHand.finger_actor[i][j], PxTransform(PxVec3(newHand.halfHeight[i][j], 0.0, 0.0)));
				//newHand.finger_joint[i][j]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
				//newHand.finger_joint[i][j]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
				newHand.finger_joint[i][j]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);

			}


				newHand.finger_joint[i][1] = PxD6JointCreate(*gPhysicsSDK, newHand.finger_actor[i][0], PxTransform(PxVec3(-newHand.halfHeight[i][0], 0.0, 0.0)), newHand.finger_actor[i][1], PxTransform(PxVec3(newHand.halfHeight[i][1], 0.0, 0.0)));
				//newHand.finger_joint[i][j]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
				newHand.finger_joint[i][1]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
				newHand.finger_joint[i][1]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);

			boneType = static_cast<Leap::Bone::Type>(0);
			bone = (*fl).bone(boneType);
			Leap::Vector attachPos = handTransform.transformPoint(bone.prevJoint());
			PxVec3 attachPosMM = PxVec3(attachPos.x / 100.0, attachPos.y / 100.0, -attachPos.z / 100.0 );
			if (hand.isLeft())
			{
				attachPosMM = PxVec3(-attachPos.x / 100.0, attachPos.y / 100.0 , -attachPos.z / 100.0 );
			}
			newHand.finger_joint[i][0] = PxD6JointCreate(*gPhysicsSDK, newHand.palm, PxTransform(attachPosMM), newHand.finger_actor[i][0], PxTransform(PxVec3(newHand.halfHeight[i][0], newHand.fingerWidth[i][0] / 4.0f, 0.0), PxQuat(-PxHalfPi, PxVec3(0, 1, 0))));

			//finger_joint[i][0]->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

			//newHand.finger_joint[i][0]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
			newHand.finger_joint[i][0]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
			newHand.finger_joint[i][0]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
		}

	}
	*/


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

		Leap::Matrix handTransform = hand.basis();
		handTransform.origin = hand.palmPosition();
		PxQuat prevQuat,handQuat;
		if (hand.isLeft())
		{
			handTransform.xBasis = -handTransform.xBasis;
		}


		handTransform = handTransform.rigidInverse();


		PxVec3 col1 ,col2, col3;
		PxMat33 flipmat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });
		if (hand.isLeft())
		{
			col1 = PxVec3(-hand.basis().xBasis.x, -hand.basis().xBasis.y, -hand.basis().xBasis.z);
			col2 = PxVec3(hand.basis().yBasis.x, hand.basis().yBasis.y, hand.basis().yBasis.z);
			col3 = PxVec3(hand.basis().zBasis.x, hand.basis().zBasis.y, hand.basis().zBasis.z);
		}
		else
		{
			col1 = PxVec3(hand.basis().xBasis.x, hand.basis().xBasis.y, hand.basis().xBasis.z);
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

	PxD6JointDrive  drive[4];  
	drive[3] = PxD6JointDrive(1000, 0.01, PX_MAX_F32, true);
	drive[2] = PxD6JointDrive(1000, 0.01, PX_MAX_F32, true);
	drive[1] = PxD6JointDrive(1000, 0.01, PX_MAX_F32, true);
	drive[0] = PxD6JointDrive(1000.0, 0.01, PX_MAX_F32, true);


	int i = 0;
	handQuat = quat;
	Leap::FingerList fingers = hand.fingers();
	for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); fl++, i++)
	{
		Leap::Bone bone;
		Leap::Bone::Type boneType;

		Leap::Matrix localFingerBasis;
		PxMat33 rotatemat;
		actor.isExtended[i] = (*fl).isExtended();
		Leap::Vector tip = (*fl).tipPosition();
		actor.fingerTipPosition[i] = PxVec3(tip.x, tip.y, tip.z);
		//actor.finger_tip_achor[i]->setKinematicTarget(PxTransform(leapToWorld((*fl).tipPosition())));

			for (int j = 0; j < 4; j++)
			{
				
				if (i != 0 || j != 0)
				{
					boneType = static_cast<Leap::Bone::Type>(j);
					bone = (*fl).bone(boneType);
					Leap::Matrix bonebasis = bone.basis();

					if (hand.isLeft())
						bonebasis.xBasis = -bonebasis.xBasis;

					localFingerBasis = bonebasis;

					PxVec3 col1 = PxVec3(localFingerBasis.xBasis.x, localFingerBasis.xBasis.y, localFingerBasis.xBasis.z);
					PxVec3 col2 = PxVec3(localFingerBasis.yBasis.x, localFingerBasis.yBasis.y, localFingerBasis.yBasis.z);
					PxVec3 col3 = PxVec3(localFingerBasis.zBasis.x, localFingerBasis.zBasis.y, localFingerBasis.zBasis.z);
					rotatemat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });

					PxQuat quat = PxQuat(rotatemat*PxMat33(col1, col2, col3)*rotatemat);
					actor.finger_joint_actor[i][j]->setKinematicTarget(PxTransform(leapToWorld(bone.center()), quat));
					actor.finger_joint[i][j]->setDrive(PxD6Drive::eSWING, drive[j]);
					actor.finger_joint[i][j]->setDrive(PxD6Drive::eX, drive[j]);
					actor.finger_joint[i][j]->setDrive(PxD6Drive::eY, drive[j]);
					actor.finger_joint[i][j]->setDrive(PxD6Drive::eZ, drive[j]);

					actor.leapJointPosition[i * 4 + j] = PxVec3(bone.prevJoint().x, bone.prevJoint().y, bone.prevJoint().z);
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
			gScene->removeActor(*actor.finger_joint_actor[i][j]);
			actor.finger_actor[i][j]->release();
			actor.finger_joint_actor[i][j]->release();
			actor.finger_joint[i][j]->release();
		}
	}

	delete[] actor.leapJointPosition;
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

PxRigidDynamic* LeapClass::createCylinder(PxReal radius, PxReal halfHeight, PxVec3 pos,PxQuat quat, PxAggregate* aggregate)
{
	if (halfHeight == 0)
		halfHeight = 0.1;
	PxTransform transform(pos, quat);
	PxMaterial* mMaterial = gPhysicsSDK->createMaterial(1.0, 1.0, 1.0);
	PxReal         density = 1.0f;
	PxCapsuleGeometry geometry(radius, halfHeight);
	PxRigidDynamic *actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);


//	actor->setLinearVelocity(PxVec3(0, 0, 0));
	actor->setSolverIterationCounts(16, 8);
	if (!actor)
		cerr << "create actor failed!" << endl;
	//aggregate->addActor(*actor);

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
			if (hand.grabStrength() > 0.7 || hand.pinchStrength() > 0.7)
			{
				try {
					PxVec3 handPos = leapToWorld(hand.palmPosition());
					PxVec3 actorPos = (*ii).second->getGlobalPose().p;
					PxVec3 d = handPos - actorPos;
					Leap::Vector n = hand.basis().yBasis;
					PxVec3 pxn = PxVec3(n.x, n.y, n.z);
					PxVec3 dn = d.getNormalized();

					if (d.dot(pxn) > 0)
						forces[(*ii).first] = dn*1.5;
					else
						forces[(*ii).first] = (PxVec3(0, 0, 0));
				}
				catch (exception ex)
				{
				}

			}
			else
			{
				forces[(*ii).first] = (PxVec3(0, 0, 0));
			}
		}

	}
	return forces;

}

PxVec3 LeapClass::getWristPosition(int id)
{
	Frame frame = controller.frame();
	Hand hand = frame.hand(id);
	if (hand.isValid())
	{
		PxVec3 converted;
		converted.x = hand.wristPosition().x / 100.f;
		converted.y = hand.wristPosition().y / 100.f - 1.0f;
		converted.z = -hand.wristPosition().z / 100.f - 2.7f;
		return converted;
	}
	else
	{
		return PxVec3(0, 0, 0);
	}

}