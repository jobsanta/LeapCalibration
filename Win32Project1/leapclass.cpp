#include "leapclass.h"
using namespace Leap;

#define _joint


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
		shape->setQueryFilterData(filterData);
	}
}

void LeapClass::connect()
{
	rightHandMirrored = false;
	leftHandMirrored = false;
	nolosegripmode = false;

	globalHandMode = HandMode::Normal;
	while (!controller.isConnected());
}

bool LeapClass::captureFingerTip(float * x, float *y, float*z)
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
		PxVec3 result = leapToWorld(fingers[2].tipPosition(), HandMode::Normal);
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

#pragma region old
	//FingerList fingers = hand.fingers();
	//Leap::PointableList pointables = frame.pointables();
	//Leap::FingerList fingers = frame.fingers();
	//Leap::ToolList tools = frame.tools();
	//if (normalHand)
	//{
	//	if (!hands.isEmpty() && mHandList.size() == 0)
	//	{
	//		for (int i = 0; i < hands.count(); i++)
	//		{
	//			if (hands[i].isValid() && hands[i].palmPosition().y > 10.0f)
	//			{
	//					createHand(hands[i], handlength);
	//			}
	//		}
	//		//createRefHand(hands[0]);
	//	}
	//	if (hands.isEmpty() && mHandList.size() > 0)
	//	{
	//		for (int i = 0; i < mHandList.size(); i++)
	//		{
	//			deleteHand(mHandList[i]);
	//		}
	//		mHandList.clear();
	//	}
	//	int hands_count = hands.count();
	//	if (hands_count != (int)mHandList.size())
	//	{
	//		if (hands.count() > (int)mHandList.size())
	//		{
	//			std::vector<int> tobeDelete;
	//			for (int i = 0; i < mHandList.size(); i++)
	//			{
	//				if (frame.hand(mHandList[i].id).isValid() && frame.hand(mHandList[i].id).palmPosition().y > 10.0f)
	//				{
	//					updateHand(frame.hand(mHandList[i].id), mHandList[i]);
	//				}
	//				else
	//				{
	//					deleteHand(mHandList[i]);
	//					tobeDelete.push_back(i);
	//				}
	//			}
	//			int size = tobeDelete.size();
	//			for (int i = 0; i < size; i++)
	//			{
	//				mHandList.erase(mHandList.begin() + (int)tobeDelete.back());
	//				tobeDelete.pop_back();
	//			}
	//			tobeDelete.clear();
		//			for (int i = 0; i < hands.count(); i++)
	//			{
	//				bool tracked = false;
	//				for (int j = 0; j < mHandList.size() && tracked == false; j++)
	//				{
	//					if (hands[i].id() == mHandList[j].id)
	//					{
	//						tracked = true;
	//					}
	//				}
	//				if (tracked == false)
	//				{
	//					if (hands[i].isValid() && hands[i].palmPosition().y > 10.0f)
	//					{
	//						createHand(hands[i], handlength);
	//					}
	//				}
	//			}
	//		}
	//		else if (hands_count < (int)mHandList.size())
	//		{
	//			std::vector<int> tobeDelete;
	//			for (int i = 0; i < mHandList.size(); i++)
	//			{
	//				if (frame.hand(mHandList[i].id).isValid() && frame.hand(mHandList[i].id).palmPosition().y > 10.0f)
	//				{
	//					updateHand(frame.hand(mHandList[i].id), mHandList[i]);
	//				}
	//				else
	//				{
	//					deleteHand(mHandList[i]);
	//					tobeDelete.push_back(i);
	//				}
	//			}
	//			int size = tobeDelete.size();
	//			for (int i = 0; i < size; i++)
	//			{
	//				mHandList.erase(mHandList.begin() + (int)tobeDelete.back());
	//				tobeDelete.pop_back();
	//			}
	//			tobeDelete.clear();
	//		}
	//	}
	//	if (hands.count() == (int)mHandList.size() && hands.count() != 0)
	//	{
	//		for (int i = 0; i < (int)mHandList.size(); i++)
	//		{
	//			std::vector<int> tobeDelete;
	//			for (int i = 0; i < mHandList.size(); i++)
	//			{
	//				if (frame.hand(mHandList[i].id).isValid() && frame.hand(mHandList[i].id).palmPosition().y > 10.0f)
	//				{
	//					updateHand(frame.hand(mHandList[i].id), mHandList[i]);
	//				}
	//				else
	//				{
	//					deleteHand(mHandList[i]);
	//					tobeDelete.push_back(i);
	//				}
	//			}
	//			int size = tobeDelete.size();
	//			for (int i = 0; i < size; i++)
	//			{
	//				mHandList.erase(mHandList.begin() + (int)tobeDelete.back());
	//				tobeDelete.pop_back();
	//			}
	//			tobeDelete.clear();
	//		}
	//	}
	//}
	//else if (!normalHand)
	//{
	//	for (int i = 0; i < mHandList.size(); i++)
	//	{
	//		deleteHand(mHandList[i]);
	//	}
	//	mHandList.clear();
	//}
	//if (mirrorHand)
	//{
	//	if (!hands.isEmpty() && mMirrorHandList.size() == 0)
	//	{
	//		for (int i = 0; i < hands.count(); i++)
	//		{
	//			if (hands[i].isValid() && hands[i].palmPosition().y > 10.0f)
	//			{
	//				createMirrorHand(hands[i], handlength);
	//			}
	//		}
	//		//createRefHand(hands[0]);
	//	}
	//	if (hands.isEmpty() && mMirrorHandList.size() > 0)
	//	{
	//		for (int i = 0; i < mMirrorHandList.size(); i++)
	//		{
	//			deleteHand(mMirrorHandList[i]);
	//		}
	//		mMirrorHandList.clear();
	//	}
	//	int hands_count = hands.count();
	//	if (hands_count != (int)mMirrorHandList.size())
	//	{
	//		if (hands.count() > (int)mMirrorHandList.size())
	//		{
	//			std::vector<int> tobeDelete;
	//			for (int i = 0; i < mMirrorHandList.size(); i++)
	//			{
	//				if (frame.hand(mMirrorHandList[i].id).isValid() && frame.hand(mMirrorHandList[i].id).palmPosition().y > 10.0f)
	//				{
	//					updateMirrorHand(frame.hand(mMirrorHandList[i].id), mMirrorHandList[i]);
	//				}
	//				else
	//				{
	//					deleteHand(mMirrorHandList[i]);
	//					tobeDelete.push_back(i);
	//				}
	//			}
	//			int size = tobeDelete.size();
	//			for (int i = 0; i < size; i++)
	//			{
	//				mMirrorHandList.erase(mMirrorHandList.begin() + (int)tobeDelete.back());
	//				tobeDelete.pop_back();
	//			}
	//			tobeDelete.clear();
	//			for (int i = 0; i < hands.count(); i++)
	//			{
	//				bool tracked = false;
	//				for (int j = 0; j < mMirrorHandList.size() && tracked == false; j++)
	//				{
	//					if (hands[i].id() == mMirrorHandList[j].id)
	//					{
	//						tracked = true;
	//					}
	//				}
	//				if (tracked == false)
	//				{
	//					if (hands[i].isValid() && hands[i].palmPosition().y > 10.0f)
	//					{
	//						createMirrorHand(hands[i], handlength);
	//					}
	//				}
	//			}
	//		}
	//		else if (hands_count < (int)mMirrorHandList.size())
	//		{
	//			std::vector<int> tobeDelete;
	//			for (int i = 0; i < mMirrorHandList.size(); i++)
	//			{
	//				if (frame.hand(mMirrorHandList[i].id).isValid() && frame.hand(mMirrorHandList[i].id).palmPosition().y > 10.0f)
	//				{
	//					updateMirrorHand(frame.hand(mMirrorHandList[i].id), mMirrorHandList[i]);
	//				}
	//				else
	//				{
	//					deleteHand(mMirrorHandList[i]);
	//					tobeDelete.push_back(i);
	//				}
	//			}
	//			int size = tobeDelete.size();
	//			for (int i = 0; i < size; i++)
	//			{
	//				mMirrorHandList.erase(mMirrorHandList.begin() + (int)tobeDelete.back());
	//				tobeDelete.pop_back();
	//			}
	//			tobeDelete.clear();
	//		}
	//	}
	//	if (hands.count() == (int)mMirrorHandList.size() && hands.count() != 0)
	//	{
	//		for (int i = 0; i < (int)mMirrorHandList.size(); i++)
	//		{
	//			std::vector<int> tobeDelete;
	//			for (int i = 0; i < mMirrorHandList.size(); i++)
	//			{
	//				if (frame.hand(mMirrorHandList[i].id).isValid() && frame.hand(mMirrorHandList[i].id).palmPosition().y > 10.0f)
	//				{
	//					updateMirrorHand(frame.hand(mMirrorHandList[i].id), mMirrorHandList[i]);
	//				}
	//				else
	//				{
	//					deleteHand(mMirrorHandList[i]);
	//					tobeDelete.push_back(i);
	//				}
	//			}
	//			int size = tobeDelete.size();
	//			for (int i = 0; i < size; i++)
	//			{
	//				mMirrorHandList.erase(mMirrorHandList.begin() + (int)tobeDelete.back());
	//				tobeDelete.pop_back();
	//			}
	//			tobeDelete.clear();
	//		}
	//	}
	//}
	//else if (!mirrorHand)
	//{
	//	for (int i = 0; i < mMirrorHandList.size(); i++)
	//	{
	//		deleteHand(mMirrorHandList[i]);
	//	}
	//	mMirrorHandList.clear();
	//}
#pragma endregion

	std::vector<int> tobeDelete;
	for (int i = 0; i < mHandList.size(); i++)
	{		
		if (mHandList[i]->toBeDelete)
		{
			deleteHand(mHandList[i]);
			tobeDelete.push_back(i);
		}
		else
		{
			if (frame.hand(mHandList[i]->id).isValid() && frame.hand(mHandList[i]->id).palmPosition().y > 10.0f)
			{
				updateHands(frame.hand(mHandList[i]->id), mHandList[i]);
			}
			else
			{
				deleteHand(mHandList[i]);
				tobeDelete.push_back(i);
			}
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
			if (hands[i].id() == mHandList[j]->id)
			{
				tracked = true;
			}
		}
		if (tracked == false)
		{
			if (hands[i].isValid() && hands[i].palmPosition().y > 10.0f)
			{
				createHands(hands[i]);
			}
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

PxRigidDynamic* LeapClass::CreateBox(PxVec3 dimension, PxVec3 pose, PxQuat quat)
{
	PxMaterial* mMaterial = gPhysicsSDK->createMaterial(1.0, 1.0, 0.1);
	PxReal density = 1.0f;
	PxTransform transform(pose, quat);
	PxBoxGeometry geometry(dimension);
	//mMaterial = gPhysicsSDK->createMaterial(0, 0, 0.5);
	PxRigidDynamic* actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);
	actor->setSolverIterationCounts(32, 255);
	//actor->setMass(1.0);
	//actor->setMassSpaceInertiaTensor(PxVec3(1.0, 1.0, 1.0));
	setupFiltering(actor, FilterGroup::eBox, FilterGroup::eBox | FilterGroup::eFinger | FilterGroup::eWall | FilterGroup::eHand | FilterGroup::eTarget);
	actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	PxU32 nShapes = actor->getNbShapes();
	gScene->addActor(*actor);

	return actor;
}

void LeapClass::InitPhysx(PxPhysics* sdk, PxScene* scene)
{
	gPhysicsSDK = sdk;
	gScene = scene;

	normalHand = true;
	mirrorHand = false;

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

PxVec3 LeapClass::leapToWorld(Leap::Vector bone_center, HandMode::Enum handmode)
{
	PxVec3 converted;
	converted.x = bone_center.x / 100.f;
	converted.y = bone_center.y / 100.f - 2.7;
	if (handmode == HandMode::Normal)
		converted.z = -bone_center.z / 100.f - 2.9;
	else if (handmode == HandMode::Mirror)
		converted.z = bone_center.z / 100.f + 2.9;
	float normalized_y = y_head - 1.7;

	if (handmode == HandMode::Gogo)
	{
		converted.z = -bone_center.z / 100.f - 2.9;
		if (OFFAXIS)
			normalized_y = normalized_y - 2.0f;
		PxVec3 head(x_head, normalized_y, z_head);
		PxVec3 r = converted - head;
		float R_r = r.magnitude();
		float R_v = R_r;
		if (R_r > 1)
		{
			R_v = R_r + (R_r - 1)*(R_r - 1) / 2;
		}

		converted = (r.getNormalized()*R_v + head);
	}

	converted.x = (x_head - converted.x)*z_head / (z_head - converted.z);
	converted.y = (normalized_y - converted.y)*z_head / (z_head - converted.z);

	float sx = -(converted.x - x_head) / 3.0f;
	float sy = -(converted.y - normalized_y) / 1.7f;

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

	MatrixXf a(2, 2);
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
		a(0, 0) = -xScale*xx - rl *zx + sx*zx;
		a(1, 0) = -yScale*yx + sy*zx - tb*zx;
		a(0, 1) = -xScale*xy - rl*zy + sx*zy;
		a(1, 1) = -yScale*yy + sy*zy - tb*zy;

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

void LeapClass::createHand(Hand hand, bool goinUp, bool isInTransit, float handLength, HandMode::Enum handMode, PxRigidDynamic* grab,int grabType)
{
	PxU32 nbActors = 45;

	float mass[4] = { 0.1,0.1,0.1,0.1 };
	PxVec3 I_tensor = PxVec3(0.05, 0.05, 0.05);
	bool selfCollision = true;
	handActor* newHand = new handActor();

	newHand->aggregate = gPhysicsSDK->createAggregate(nbActors, selfCollision);
	newHand->id = hand.id();

	if (handMode == HandMode::Normal || handMode == HandMode::Gogo)
		newHand->isInMirror = false;
	else if (handMode == HandMode::Mirror)
		newHand->isInMirror = true;

	newHand->wristPosition = hand.wristPosition();
	newHand->isInTransit = isInTransit;
	newHand->alphaValue = 0.5f;
	newHand->toBeDelete = false;
	newHand->goingUp = goinUp;
	newHand->grabActor = grab;
	newHand->pinchStrength = new float[1];
	newHand->grabType = grabType;

	Leap::Matrix handTransform = hand.basis();
	handTransform.origin = hand.palmPosition();
	newHand->previousTime = (double)hand.frame().timestamp() / 1000000.0;
	newHand->startTransitTime = (double)hand.frame().timestamp() / 1000000.0;

	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 3; j++)
		{
			newHand->previousSpringHandAngle[i][j] = PxHalfPi;
			newHand->previousHandAngle[i][j] = PxHalfPi;
		}

	if (hand.isLeft())
	{
		handTransform.xBasis = -handTransform.xBasis;
	}

	handTransform = handTransform.rigidInverse();

	PxVec3 col1, col2, col3;
	PxMat33 flipmat;
	if (handMode == HandMode::Normal || handMode == HandMode::Gogo)
		flipmat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });
	else if (handMode == HandMode::Mirror)
		flipmat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 });

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
	newHand->handTransform = PxTransform(leapToWorld(hand.palmPosition(), handMode), quat);
	PxMaterial* material = gPhysicsSDK->createMaterial(1.0f, 1.0f, 0.1f);
	newHand->palm = PxCreateDynamic(*gPhysicsSDK, PxTransform(leapToWorld(hand.palmPosition(), handMode), quat), PxBoxGeometry((hand.palmWidth() - hand.fingers()[0].width()) / 200, 0.1, hand.palmWidth() / 200), *material, 1.0f);
	if (newHand->palm == NULL)
	{
		return;
	}
	newHand->palm->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
	PxShape* shapes;
	newHand->palm->getShapes(&shapes, 1);
	shapes->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	shapes->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	setupFiltering(newHand->palm, FilterGroup::eHand, FilterGroup::eBox | FilterGroup::eTarget);

	newHand->aggregate->addActor(*newHand->palm);

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

	newHand->palmDimension = PxVec3(hand.palmWidth() / 120.0f, hand.palmWidth() / 100.0f, hand.palmWidth() / 400.0f);
	newHand->leapJointPosition = std::vector<PxVec3>(20);
	newHand->fingerTipPosition = new PxVec3[5];
	newHand->isExtended = new bool[5];

	int i = 0;
	Leap::FingerList fingers = hand.fingers();
	for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); fl++, i++)
	{
		Leap::Bone bone;
		Leap::Bone::Type boneType;

		Leap::Matrix localFingerBasis;
		PxMat33 rotatemat;
		Leap::Matrix preBone = handTransform;
		newHand->isExtended[i] = (*fl).isExtended();
		Leap::Vector tip = (*fl).tipPosition();
		newHand->fingerTipPosition[i] = PxVec3(tip.x, tip.y, tip.z);
		

		if (i == 0)
		{
			boneType = static_cast<Leap::Bone::Type>(1);
			bone = (*fl).bone(boneType);

			newHand->halfHeight[i][1] = bone.length() / 200.0f;
			newHand->fingerWidth[i][1] = factor*bone.width() / 100.0f;
			newHand->finger_actor[i][1] = gPhysicsSDK->createRigidDynamic(PxTransform(leapToWorld(bone.center(), handMode)));
			PxShape* box = newHand->finger_actor[i][1]->createShape(PxBoxGeometry(newHand->halfHeight[i][1], newHand->fingerWidth[i][1] / 2.0f, newHand->fingerWidth[i][1] / 4.0f), *material);
			box->setLocalPose(PxTransform(PxQuat(-PxHalfPi, PxVec3(0, 1, 0))));
			newHand->finger_actor[i][1]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			newHand->finger_actor[i][1]->setMass(mass[1]);
			newHand->finger_actor[i][1]->setMassSpaceInertiaTensor(I_tensor);

			setupFiltering(newHand->finger_actor[i][1], FilterGroup::eFinger, FilterGroup::eBox | FilterGroup::eTarget);
			newHand->aggregate->addActor(*newHand->finger_actor[i][1]);

			Vector joint = preBone.transformPoint(bone.prevJoint()) / 100.0;
			if (handMode == HandMode::Normal || handMode == HandMode::Gogo)
				newHand->finger_joint[i][1] = PxD6JointCreate(*gPhysicsSDK, newHand->palm, PxTransform(PxVec3(joint.x, joint.y, -joint.z)), newHand->finger_actor[i][1], PxTransform(PxVec3(0, 0, -newHand->halfHeight[i][1])));
			else if (handMode == HandMode::Mirror)
				newHand->finger_joint[i][1] = PxD6JointCreate(*gPhysicsSDK, newHand->palm, PxTransform(PxVec3(joint.x, joint.y, joint.z)), newHand->finger_actor[i][1], PxTransform(PxVec3(0, 0, newHand->halfHeight[i][1])));

			newHand->finger_joint[i][1]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
			newHand->finger_joint[i][1]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
			newHand->finger_joint[i][1]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
			newHand->leapJointPosition[i*4+0] = LeapVectoPxVec3(bone.center());
			newHand->leapJointPosition[i * 4 + 1] = LeapVectoPxVec3(bone.center());


			for (int j = 2; j < 4; j++)
			{
				boneType = static_cast<Leap::Bone::Type>(j);
				bone = (*fl).bone(boneType);

				newHand->halfHeight[i][j] = bone.length() / 200.0f;
				newHand->fingerWidth[i][j] = factor*bone.width() / 100.0f;
				newHand->finger_actor[i][j] = createCylinder(bone.width() / 200.0f, newHand->halfHeight[i][j], leapToWorld(bone.center(), handMode), PxQuat(-PxHalfPi, PxVec3(0, 1, 0)), newHand->aggregate);
				newHand->finger_actor[i][j]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
				newHand->finger_actor[i][j]->setMass(mass[j]);
				newHand->finger_actor[i][j]->setMassSpaceInertiaTensor(I_tensor);
				setupFiltering(newHand->finger_actor[i][j], FilterGroup::eFinger, FilterGroup::eBox | FilterGroup::eTarget);

				newHand->aggregate->addActor(*newHand->finger_actor[i][j]);


				if (handMode == HandMode::Normal || handMode == HandMode::Gogo)
					newHand->finger_joint[i][j] = PxD6JointCreate(*gPhysicsSDK, newHand->finger_actor[i][j - 1], PxTransform(PxVec3(0, 0, newHand->halfHeight[i][j - 1])), newHand->finger_actor[i][j], PxTransform(PxVec3(0, 0, -newHand->halfHeight[i][j])));
				else if (handMode == HandMode::Mirror)
					newHand->finger_joint[i][j] = PxD6JointCreate(*gPhysicsSDK, newHand->finger_actor[i][j - 1], PxTransform(PxVec3(0, 0, -newHand->halfHeight[i][j - 1])), newHand->finger_actor[i][j], PxTransform(PxVec3(0, 0, newHand->halfHeight[i][j])));

				newHand->finger_joint[i][j]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
				if (j == 2)
					newHand->finger_joint[i][j]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);

				newHand->leapJointPosition[i * 4 + j] = LeapVectoPxVec3(bone.center());
			}
		}
		else
		{
			boneType = static_cast<Leap::Bone::Type>(0);
			bone = (*fl).bone(boneType);

			newHand->halfHeight[i][0] = bone.length() / 200.0f;
			newHand->fingerWidth[i][0] = factor*bone.width() / 100.0f;
			newHand->finger_actor[i][0] = gPhysicsSDK->createRigidDynamic(PxTransform(leapToWorld(bone.center(), handMode)));
			PxShape* box = newHand->finger_actor[i][0]->createShape(PxBoxGeometry(newHand->halfHeight[i][0], newHand->fingerWidth[i][0] / 2.0f, newHand->fingerWidth[i][0] / 4.0f), *material);
			box->setLocalPose(PxTransform(PxQuat(-PxHalfPi, PxVec3(0, 1, 0))));
			newHand->finger_actor[i][0]->setMass(mass[0]);
			newHand->finger_actor[i][0]->setMassSpaceInertiaTensor(I_tensor);

			newHand->finger_actor[i][0]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			setupFiltering(newHand->finger_actor[i][0], FilterGroup::eFinger, FilterGroup::eBox | FilterGroup::eTarget);
			newHand->aggregate->addActor(*newHand->finger_actor[i][0]);

			Vector joint = preBone.transformPoint(bone.prevJoint()) / 100.0;
			if (handMode == HandMode::Normal || handMode == HandMode::Gogo)
				newHand->finger_joint[i][0] = PxD6JointCreate(*gPhysicsSDK, newHand->palm, PxTransform(PxVec3(joint.x, joint.y, -joint.z)), newHand->finger_actor[i][0], PxTransform(PxVec3(0, 0, -newHand->halfHeight[i][0])));
			else if (handMode == HandMode::Mirror)
				newHand->finger_joint[i][0] = PxD6JointCreate(*gPhysicsSDK, newHand->palm, PxTransform(PxVec3(joint.x, joint.y, joint.z)), newHand->finger_actor[i][0], PxTransform(PxVec3(0, 0, newHand->halfHeight[i][0])));

			newHand->finger_joint[i][0]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
			newHand->finger_joint[i][0]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
			newHand->finger_joint[i][0]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);

			newHand->leapJointPosition[i * 4] = LeapVectoPxVec3(bone.center());

			for (int j = 1; j < 4; j++)
			{
				boneType = static_cast<Leap::Bone::Type>(j);
				bone = (*fl).bone(boneType);

				newHand->halfHeight[i][j] = bone.length() / 200.0f;
				newHand->fingerWidth[i][j] = factor*bone.width() / 100.0f;
				newHand->finger_actor[i][j] = createCylinder(bone.width() / 200.0f, newHand->halfHeight[i][j], leapToWorld(bone.center(), handMode), PxQuat(-PxHalfPi, PxVec3(0, 1, 0)), newHand->aggregate);
				newHand->finger_actor[i][j]->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
				newHand->finger_actor[i][j]->setMass(mass[j]);
				newHand->finger_actor[i][j]->setMassSpaceInertiaTensor(I_tensor);
				setupFiltering(newHand->finger_actor[i][j], FilterGroup::eFinger, FilterGroup::eFinger | FilterGroup::eBox | FilterGroup::eTarget);

				newHand->aggregate->addActor(*newHand->finger_actor[i][j]);

				if (handMode == HandMode::Normal || handMode == HandMode::Gogo)
					newHand->finger_joint[i][j] = PxD6JointCreate(*gPhysicsSDK, newHand->finger_actor[i][j - 1], PxTransform(PxVec3(0, 0, newHand->halfHeight[i][j - 1])), newHand->finger_actor[i][j], PxTransform(PxVec3(0, 0, -newHand->halfHeight[i][j])));
				else if (handMode == HandMode::Mirror)
					newHand->finger_joint[i][j] = PxD6JointCreate(*gPhysicsSDK, newHand->finger_actor[i][j - 1], PxTransform(PxVec3(0, 0, -newHand->halfHeight[i][j - 1])), newHand->finger_actor[i][j], PxTransform(PxVec3(0, 0, newHand->halfHeight[i][j])));

				newHand->finger_joint[i][j]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
				if (j == 1)
					newHand->finger_joint[i][j]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);

				newHand->leapJointPosition[i * 4 + j] = LeapVectoPxVec3(bone.center());
			}
		}
	}

	gScene->addAggregate(*(newHand->aggregate));

	mHandList.push_back(newHand);
}

void LeapClass::createHands(Hand hand)
{
	if (globalHandMode == HandMode::Normal)
	{
		if (hand.isRight())
		{
			if (rightHandMirrored)
			{
				createHand(hand, false, false, 1, HandMode::Mirror,NULL,0);
			}
			else
			{
				createHand(hand, false, false, 1, HandMode::Normal, NULL,0);
			}
		}
		else if (hand.isLeft())
		{
			if (leftHandMirrored)
			{
				createHand(hand, false, false, 1, HandMode::Mirror, NULL,0);
			}
			else
			{
				createHand(hand, false, false, 1, HandMode::Normal, NULL,0);
			}
		}
	}
	else if (globalHandMode == HandMode::Gogo)
	{
		createHand(hand, false, false, 1, HandMode::Gogo, NULL,0);
	}
}

void LeapClass::updateHands(Hand hand, handActor* actor)
{
	if (globalHandMode == HandMode::Normal)
	{
		if (hand.isRight())
		{
			if (actor->isInMirror)
			{
				updateHand(hand, actor, HandMode::Mirror);
			}
			else
			{
				updateHand(hand, actor, HandMode::Normal);
			}
		}
		else if (hand.isLeft())
		{
			if (actor->isInMirror)
			{
				updateHand(hand, actor, HandMode::Mirror);
			}
			else
			{
				updateHand(hand, actor, HandMode::Normal);
			}
		}
	}
	else if (globalHandMode == HandMode::Gogo)
	{
		updateHand(hand, actor, HandMode::Gogo);
	}
}

void LeapClass::updateHand(Hand hand, handActor* actor, HandMode::Enum handmode)
{
	//if (hand.confidence() < 0.3)
	//	return;
	float springTerm = 5000;
	float dampingTerm = 10;
	float physxSpring = 5000;
	Leap::Matrix handTransform = hand.basis();
	handTransform.origin = hand.palmPosition();
	PxQuat prevQuat, handQuat;

	actor->pinchStrength[0] = hand.grabStrength();
	actor->wristPosition = hand.wristPosition();
	if (hand.isLeft())
	{
		handTransform.xBasis = -handTransform.xBasis;
	}

	handTransform = handTransform.rigidInverse();

	PxVec3 col1, col2, col3;
	PxMat33 flipmat;
	if (handmode == HandMode::Normal || handmode == HandMode::Gogo)
		flipmat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });
	else if (handmode == HandMode::Mirror)
		flipmat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 });

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
	PxQuat quat_hand = PxQuat(rot);
	PxVec3 palmPos = leapToWorld(hand.palmPosition(), handmode);
	PxTransform transform_update(palmPos, quat_hand);

	actor->handTransform = transform_update;

	Leap::FingerList fingers = hand.fingers();
	PxVec3 tipPos[5];
	for (int i = 0; i < 5; i++)
	{
		tipPos[i] = leapToWorld(fingers[i].tipPosition(), handmode);
	}
	bool graspFinger[5] = { false };
	bool graspDetected = false;
	bool pinchDetected = false;

	/*
	if (hand.pinchStrength() > 0.2)
	{
		if (actor->grabActor == NULL)
		{
			if (handmode == HandMode::Gogo)
			{
				PxVec3 center = leapToWorld(hand.sphereCenter(), handmode);
				actor->grabActor = queryScene(PxTransform(center), PxSphereGeometry(1.0f));
				if (actor->grabActor != NULL)
				{

					float st = hand.pinchStrength();
					actor->pinchStrength[0] = st;
					actor->grabActorStartPos = actor->grabActor->getGlobalPose();

					actor->grabActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
					actor->grabActor->userData = static_cast<void*>(actor->pinchStrength);
					if (center.y < 1.25f)
						center.y = 1.25f;

					actor->grabHandStartPos = PxTransform(center, quat_hand);
					actor->grabActor->setKinematicTarget(PxTransform(actor->grabActorStartPos));

				}

			}
			else
			{
				actor->grabActor = queryScene(PxTransform(tipPos[0]), PxSphereGeometry(0.1f));
				if (actor->grabActor != NULL)
				{

					PxVec3 center = PxVec3(0, 0, 0);
					PxVec3 thumb = tipPos[0];

					int count = 1;
					for (int i = 1; i < 5; i++)
					{
						if (queryScene(PxTransform(tipPos[i]), PxSphereGeometry(0.1f)) != NULL)
						{
							center += tipPos[i];
							count++;
						}

					}
					if (count == 1)
					{
						actor->grabActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
						actor->grabActor = NULL;
					}
					else
					{
						float st = hand.pinchStrength();
						actor->pinchStrength[0] = st;
						actor->grabActorStartPos = actor->grabActor->getGlobalPose();

						actor->grabActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
						actor->grabActor->userData = static_cast<void*>(actor->pinchStrength);
						center = (center + thumb) / count;
						if (center.y < 1.25f)
							center.y = 1.25f;

						actor->grabHandStartPos = PxTransform(center, quat_hand);
						actor->grabActor->setKinematicTarget(PxTransform(actor->grabActorStartPos));
					}

				}
			}

		}
		else
		{

			if (handmode == HandMode::Gogo) {
				PxVec3 center = leapToWorld(hand.sphereCenter(), handmode);
				PxQuat diff = actor->grabHandStartPos.q *quat_hand.getConjugate();
				if (center.y < 1.25f)
					center.y = 1.25f;
				float st = hand.pinchStrength();
				actor->pinchStrength[0] = st;
				actor->grabActor->userData = static_cast<float*>(actor->pinchStrength);
				actor->grabActor->setKinematicTarget(PxTransform(actor->grabActorStartPos.p + (center - actor->grabHandStartPos.p), diff.getConjugate()* actor->grabActorStartPos.q));

				for (int i = 1; i < 5; i++)
				{
					graspFinger[i] = true;
				}
			}
			else
			{
				PxVec3 thumb = tipPos[0];
				PxVec3 center = PxVec3(0, 0, 0);
				int count = 1;
				for (int i = 1; i < 5; i++)
				{
					if (queryScene(PxTransform(tipPos[i]), PxSphereGeometry(0.1f)) != NULL)
					{
						center += tipPos[i];
						graspFinger[i] = true;
						count++;
					}
					else
					{
						graspFinger[i] = false;
					}

				}

				if (count == 1 && !actor->goingUp)
				{
					actor->grabActor->userData = NULL;
					actor->grabActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
					actor->grabActor = NULL;
				}
				else if (count > 1)
				{
					PxQuat diff = actor->grabHandStartPos.q *quat_hand.getConjugate();
					center = (center + thumb) / count;
					if (center.y < 1.25f)
						center.y = 1.25f;
					float st = hand.pinchStrength();
					actor->pinchStrength[0] = st;
					actor->grabActor->userData = static_cast<float*>(actor->pinchStrength);
					actor->grabActor->setKinematicTarget(PxTransform(actor->grabActorStartPos.p + (center - actor->grabHandStartPos.p), diff.getConjugate()* actor->grabActorStartPos.q));
				}
			}


		}
	}

	if (hand.pinchStrength() <= 0.2)
	{
		if (actor->grabActor != NULL)
		{
			actor->grabActor->userData = NULL;
			actor->grabActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
			actor->grabActor = NULL;
		}
	}
	if (hand.grabStrength() > 0.1 && hand.pinchStrength() <= 0.2)
	{
		if (actor->grabActor == NULL)
		{
			PxVec3 center = leapToWorld(hand.sphereCenter(), handmode);
			actor->grabActor = queryScene(PxTransform(center), PxSphereGeometry(1.0f));

			if (actor->grabActor != NULL)
			{

				float st = hand.grabStrength();
				actor->pinchStrength[0] = st;
				actor->grabActorStartPos = actor->grabActor->getGlobalPose();

				actor->grabActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
				actor->grabActor->userData = static_cast<void*>(actor->pinchStrength);
				if (center.y < 1.25f)
					center.y = 1.25f;

				actor->grabHandStartPos = PxTransform(center, quat_hand);
				actor->grabActor->setKinematicTarget(PxTransform(actor->grabActorStartPos));

			}
		}
		else
		{
			PxVec3 center = leapToWorld(hand.sphereCenter(), handmode);
			PxQuat diff = actor->grabHandStartPos.q *quat_hand.getConjugate();
			if (center.y < 1.25f)
				center.y = 1.25f;
			float st = hand.grabStrength();
			actor->pinchStrength[0] = st;
			actor->grabActor->userData = static_cast<float*>(actor->pinchStrength);
			actor->grabActor->setKinematicTarget(PxTransform(actor->grabActorStartPos.p + (center - actor->grabHandStartPos.p), diff.getConjugate()* actor->grabActorStartPos.q));

			for (int i = 1; i < 5; i++)
			{
				graspFinger[i] = true;
			}
		}

	}
	if (hand.pinchStrength() <= 0.1 && hand.grabStrength() <= 0.2)
	{
		if (actor->grabActor != NULL)
		{
			actor->grabActor->userData = NULL;
			actor->grabActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
			actor->grabActor = NULL;
		}
	}
	*/


	if (!nolosegripmode)
	{
	if (handmode == HandMode::Gogo)
	{
		actor->alphaValue = 0.5;
		if (hand.grabStrength() > 0.4)
		{
			if (actor->grabType == 0)
			{
				PxVec3 center = palmPos + LeapVectoPxVec3(hand.palmNormal()) / 2.0f;
				actor->grabActor = queryScene(PxTransform(center), PxSphereGeometry(1.0f));
				if (actor->grabActor != NULL)
				{

					float st = 0;
					if (hand.pinchStrength() > hand.grabStrength())
						st = hand.pinchStrength();
					else
						st = hand.grabStrength();

					actor->pinchStrength[0] = st;
					actor->grabActorStartPos = actor->grabActor->getGlobalPose();

					actor->grabActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
					actor->grabActor->userData = static_cast<void*>(actor->pinchStrength);
					if (center.y < 1.25f)
						center.y = 1.25f;

					actor->grabHandStartPos = PxTransform(center, quat_hand);
					actor->grabActor->setKinematicTarget(PxTransform(actor->grabActorStartPos));
					actor->grabType = 2;

				}
			}
			else if (actor->grabType == 2)
			{
				PxVec3 center = palmPos + LeapVectoPxVec3(hand.palmNormal()) / 2.0f;
				//actor->grabActor = queryScene(PxTransform(center), PxSphereGeometry(2.0));
				//if (actor->grabActor != NULL)
				//{

				float st = 0;
				if (hand.pinchStrength() > hand.grabStrength())
					st = hand.pinchStrength();
				else
					st = hand.grabStrength();

				PxQuat diff = actor->grabHandStartPos.q *quat_hand.getConjugate();
				if (center.y < 1.25f)
					center.y = 1.25f;
				actor->pinchStrength[0] = st;
				actor->grabActor->userData = static_cast<float*>(actor->pinchStrength);
				actor->grabActor->setKinematicTarget(PxTransform(actor->grabActorStartPos.p + (center - actor->grabHandStartPos.p), diff.getConjugate()* actor->grabActorStartPos.q));


			}

		}
		else if (hand.grabStrength() < 0.4 && actor->grabType == 2)
		{
			actor->grabType = 0;
		}
	}
	else
	{
		if (hand.pinchStrength() > 0.2)
		{
			if (actor->grabType == 0 || actor->grabType == 2)
			{
				PxRigidDynamic* temp = queryScene(PxTransform(tipPos[0]), PxSphereGeometry(1));
				if (temp != NULL)
				{
					PxVec3 center = PxVec3(0, 0, 0);
					PxVec3 thumb = tipPos[0];

					int count = 1;
					for (int i = 1; i < 5; i++)
					{
						if (queryScene(PxTransform(tipPos[i]), PxSphereGeometry(1)) != NULL)
						{
							center += tipPos[i];
							count++;
							graspFinger[i] = true;
						}

					}
					if (count >= 1)
					{
						//detect first pinch
						float st = hand.pinchStrength();
						actor->pinchStrength[0] = st;
						actor->grabActor = temp;
						actor->grabActorStartPos = actor->grabActor->getGlobalPose();

						actor->grabActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
						actor->grabActor->userData = static_cast<void*>(actor->pinchStrength);
						center = (center + thumb) / count;
						if (center.y < 1.25f)
							center.y = 1.25f;

						actor->grabHandStartPos = PxTransform(center, quat_hand);
						actor->grabActor->setKinematicTarget(PxTransform(actor->grabActorStartPos));
						actor->grabType = 1;
					}
				}
			}
			else if (actor->grabType == 1)
			{
				PxVec3 thumb = tipPos[0];
				PxVec3 center = PxVec3(0, 0, 0);
				int count = 1;
				for (int i = 1; i < 5; i++)
				{
					if (queryScene(PxTransform(tipPos[i]), PxSphereGeometry(0.5)) != NULL)
					{
						center += tipPos[i];
						graspFinger[i] = true;
						count++;
					}
				}

				//if (count == 1 && !actor->goingUp)
				//{
				//	//release actor
				//	actor->grabType = 0;
				//}
				//else if (count > 1)
				//{
					//still pinch move actor
					PxQuat diff = actor->grabHandStartPos.q *quat_hand.getConjugate();
					center = (center + thumb) / count;
					if (center.y < 1.25f)
						center.y = 1.25f;
					float st = hand.pinchStrength();
					actor->pinchStrength[0] = st;
					actor->grabActor->userData = static_cast<float*>(actor->pinchStrength);
					actor->grabActor->setKinematicTarget(PxTransform(actor->grabActorStartPos.p + (center - actor->grabHandStartPos.p), diff.getConjugate()* actor->grabActorStartPos.q));
				//}
			}
		}

		if (hand.grabStrength() > 0.3)
		{
			if (actor->grabType == 0 || actor->grabType == 1)
			{

				PxVec3 n = LeapVectoPxVec3(hand.palmNormal());
				PxVec3 hcenter = palmPos + n*0.5;
				PxRigidDynamic* temp = queryScene(PxTransform(hcenter), PxSphereGeometry(1.0));
				if (temp != NULL)
				{
					PxVec3 center = hcenter;

					int count = 1;
					/*				for (int i = 0; i < 5; i++)
					{
					if (queryScene(PxTransform(tipPos[i]), PxSphereGeometry(0.1f)) != NULL)
					{
					center += tipPos[i];
					count++;
					graspFinger[i] = true;
					}

					}*/

					//detect first grasp

					float st = hand.grabStrength();
					actor->pinchStrength[0] = st;
					actor->grabActor = temp;
					actor->grabActorStartPos = actor->grabActor->getGlobalPose();

					actor->grabActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
					actor->grabActor->userData = static_cast<void*>(actor->pinchStrength);
					center = (center) / count;
					if (center.y < 1.25f)
						center.y = 1.25f;

					actor->grabHandStartPos = PxTransform(center, quat_hand);
					actor->grabActor->setKinematicTarget(PxTransform(actor->grabActorStartPos));
					actor->grabType = 2;


				}

			}
			else if (actor->grabType == 2)
			{
				PxVec3 n = LeapVectoPxVec3(hand.palmNormal());
				PxVec3 center = palmPos + n*0.5;
				int count = 1;
				//for (int i = 0; i < 5; i++)
				//{
				//	if (queryScene(PxTransform(tipPos[i]), PxSphereGeometry(0.1f)) != NULL)
				//	{
				//		center += tipPos[i];
				//		graspFinger[i] = true;
				//		count++;
				//	}
				//}

				//still pinch move actor
				PxQuat diff = actor->grabHandStartPos.q *quat_hand.getConjugate();

				center = (center) / count;
				if (center.y < 1.25f)
					center.y = 1.25f;
				float st = hand.grabStrength();
				actor->pinchStrength[0] = st;
				actor->grabActor->userData = static_cast<float*>(actor->pinchStrength);
				actor->grabActor->setKinematicTarget(PxTransform(actor->grabActorStartPos.p + (center - actor->grabHandStartPos.p), diff.getConjugate()* actor->grabActorStartPos.q));


			}

		}

		if (hand.pinchStrength() < 0.2 && actor->grabType == 1)
		{
			actor->grabType = 0;
		}
		if (hand.grabStrength() < 0.3 && actor->grabType == 2)
		{
			actor->grabType = 0;
		}

	}

	if (actor->grabType == 0)
	{

		if (actor->grabActor != NULL)
		{
			actor->grabActor->userData = NULL;
			if (actor->grabActor->isRigidDynamic() && !nolosegripmode)
				actor->grabActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
			actor->grabActor = NULL;
		}

	}


	}

	//Transfer hand in to mirror
	if (handmode == HandMode::Mirror || handmode == HandMode::Normal)
	{
		if (((palmPos.z > -SWITCHRANGE && palmPos.z < 0) || (palmPos.z < SWITCHRANGE && palmPos.z > 0)) && actor->isInTransit == false)
		{
			// hand in range start transit animation
			actor->isInTransit = true;
			actor->transitComplete = false;
			actor->startTransitTime = ((double)hand.frame().timestamp() / 1000000.0);
			if (handmode == HandMode::Normal)
				createHand(hand, true, true, 1.0f, HandMode::Mirror, actor->grabActor, actor->grabType);
			else if (handmode == HandMode::Mirror)
				createHand(hand, true, true, 1.0f, HandMode::Normal, actor->grabActor, actor->grabType);



		}
		else if (actor->isInTransit == true)
		{
			// if user move hand out of range delete actor going up keep actor going down
			if (palmPos.z < -SWITCHRANGE || palmPos.z > SWITCHRANGE)
			{
				double timediff = ((double)hand.frame().timestamp() / 1000000.0) - actor->startTransitTime;
				actor->isInTransit = false;
				if (actor->goingUp && timediff < SWITCHTIME)
				{
					actor->alphaValue = 0.0f;
					actor->toBeDelete = true;
				}
				else
				{
					actor->alphaValue = 0.5;
					actor->toBeDelete = false;
					actor->goingUp = false;
				}
			}
			else if (!actor->goingUp)
			{
				double timediff = ((double)hand.frame().timestamp() / 1000000.0) - actor->startTransitTime;
				if (timediff > SWITCHTIME)
				{
					actor->isInTransit = false;
					actor->toBeDelete = true;
				}
				else
				{
					actor->toBeDelete = false;
				}
				actor->alphaValue =0.5* (SWITCHTIME - timediff) / SWITCHTIME;
			}
			else if (actor->goingUp)
			{
				double timediff = ((double)hand.frame().timestamp() / 1000000.0) - actor->startTransitTime;

				if (timediff > SWITCHTIME)
				{
					actor->toBeDelete = false;
					if (hand.isLeft() && handmode == HandMode::Normal)
						leftHandMirrored = false;
					else if (hand.isLeft() && handmode == HandMode::Mirror)
						leftHandMirrored = true;
					if (hand.isRight() && handmode == HandMode::Normal)
						rightHandMirrored = false;
					else if (hand.isRight() && handmode == HandMode::Mirror)
						rightHandMirrored = true;

					if (!actor->transitComplete)
					{
						PxTransform t = transform_update;
						t.p.z = -t.p.z;

						PxRigidDynamic* touchActor = actor->grabActor;
						if (touchActor != NULL)
						{
							PxTransform trans = touchActor->getGlobalPose();
							trans.p.z = -trans.p.z;
							PxMat33 rotatemat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });
							PxQuat q = PxQuat(rotatemat*PxMat33(trans.q.getBasisVector0(), trans.q.getBasisVector1(), trans.q.getBasisVector2())*rotatemat);
							trans.q = q;
							touchActor->setKinematicTarget(trans);
							actor->grabActor->userData = NULL;
							actor->grabType = 0;
							//actor->grabActorStartPos = trans;
							//PxVec3 center = palmPos + LeapVectoPxVec3(hand.palmNormal()) / 2.0f;
							//actor->grabHandStartPos = PxTransform(center, quat_hand);
							actor->grabActor = NULL;

						}

						actor->transitComplete = true;
					}
				}
				actor->alphaValue = (timediff) / SWITCHTIME;
			}
		}
	}
	
	if (!actor->isRef)
	{
		actor->palm->setKinematicTarget(transform_update);
	}
	Leap::Matrix prevBasis = handTransform;
	PxD6JointDrive  drive[4], drivePos;
	drive[3] = PxD6JointDrive(physxSpring, 0, PX_MAX_F32, true);
	drive[2] = PxD6JointDrive(physxSpring, 0, PX_MAX_F32, true);
	drive[1] = PxD6JointDrive(physxSpring, 0, PX_MAX_F32, true);
	drive[0] = PxD6JointDrive(physxSpring, 0, PX_MAX_F32, true);
	drivePos = PxD6JointDrive(3000.0, 0, PX_MAX_F32, true);

	calculateTrackedHand(hand);
	calculateSpringHand(actor);

	float w_s[5][3];
	float w_t[5][3];
	int64_t time = hand.frame().timestamp();
	double timediff = ((double)hand.frame().timestamp() / 1000000.0) - actor->previousTime;
	if (timediff > 0)
	{
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				w_t[i][j] = (trackHandAngle[i][j] - actor->previousHandAngle[i][j]) / timediff;
				w_s[i][j] = (springHandAngle[i][j] - actor->previousSpringHandAngle[i][j]) / timediff;
			}
		}
	}
	else
	{
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				w_t[i][j] = 0;
				w_s[i][j] = 0;
			}
		}
	}

	int i = 0;

	for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); fl++, i++)
	{
		Leap::Bone bone;
		Leap::Bone::Type boneType;
		actor->isExtended[i] = (*fl).isExtended();
		Leap::Vector tip = (*fl).tipPosition();
		actor->fingerTipPosition[i] = PxVec3(tip.x, tip.y, tip.z);

		if (i != 0)
		{
			for (int j = 0; j < 4; j++)
			{
				boneType = static_cast<Leap::Bone::Type>(j);
				bone = (*fl).bone(boneType);
				Leap::Matrix bonebasis = bone.basis();
				actor->leapJointPosition[i * 4 + j] = LeapVectoPxVec3(bone.center());

				if (hand.isLeft())
					bonebasis.xBasis = -bonebasis.xBasis;

				if (j == 0)
				{
					Leap::Matrix localFingerBasis = prevBasis*bonebasis;
					PxVec3 col1, col2, col3;

					col1 = PxVec3(localFingerBasis.xBasis.x, localFingerBasis.xBasis.y, localFingerBasis.xBasis.z);
					col2 = PxVec3(localFingerBasis.yBasis.x, localFingerBasis.yBasis.y, localFingerBasis.yBasis.z);
					col3 = PxVec3(localFingerBasis.zBasis.x, localFingerBasis.zBasis.y, localFingerBasis.zBasis.z);
					PxMat33 rotatemat;
					if (handmode == HandMode::Normal || handmode == HandMode::Gogo)
						rotatemat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });
					else if (handmode == HandMode::Mirror)
						rotatemat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 });
					//test = PxVec3(test.x, test.y, -test.z);

					PxQuat quat = PxQuat(rotatemat*PxMat33(col1, col2, col3)*rotatemat);

					actor->finger_joint[i][j]->setDrive(PxD6Drive::eSLERP, drivePos);
					actor->finger_joint[i][j]->setDrivePosition(PxTransform(quat));

				}
				else if (j == 1)
				{
					actor->finger_joint[i][j]->setDrive(PxD6Drive::eTWIST, drive[j]);
					actor->finger_joint[i][j]->setDrive(PxD6Drive::eSWING, drive[j]);
					float different = ((trackHandAngle[i][j - 1] - springHandAngle[i][j - 1]) * springTerm - dampingTerm*(w_s[i][j - 1] - w_t[i][j - 1])) / physxSpring;
					float different1 = (trackHandAbduct[i] - springHandAbduct[i]) * springTerm / physxSpring;
					PxQuat a = PxQuat(springHandAngle[i][j - 1] + different - PxHalfPi, PxVec3(1, 0, 0));
					PxQuat b = PxQuat(PxHalfPi - springHandAbduct[i] - different1, PxVec3(0, 1, 0));
					if (hand.isLeft())
						b = PxQuat(-PxHalfPi + springHandAbduct[i] + different1, PxVec3(0, 1, 0));

					if (handmode == HandMode::Mirror)
					{
						a = PxQuat(-springHandAngle[i][j - 1] - different + PxHalfPi, PxVec3(1, 0, 0));
						b = PxQuat(-PxHalfPi + springHandAbduct[i] + different1, PxVec3(0, 1, 0));
						if (hand.isLeft())
							b = PxQuat(PxHalfPi - springHandAbduct[i] - different1, PxVec3(0, 1, 0));
					}

					if (!graspFinger[i])
					{
						actor->finger_joint[i][j]->setDrivePosition(PxTransform(b*a));
					}
				}
				else if (j > 0)
				{
					actor->finger_joint[i][j]->setDrive(PxD6Drive::eTWIST, drive[j]);
					float different = ((trackHandAngle[i][j - 1] - springHandAngle[i][j - 1]) * springTerm - dampingTerm*(w_s[i][j - 1] - w_t[i][j - 1])) / physxSpring;

					if (!graspFinger[i])
					{
						if (handmode == HandMode::Normal || handmode == HandMode::Gogo)
							actor->finger_joint[i][j]->setDrivePosition(PxTransform(PxQuat(springHandAngle[i][j - 1] + different - PxHalfPi, PxVec3(1, 0, 0))));
						else if (handmode == HandMode::Mirror)
							actor->finger_joint[i][j]->setDrivePosition(PxTransform(PxQuat(-springHandAngle[i][j - 1] - different + PxHalfPi, PxVec3(1, 0, 0))));

					}

				}
			}
		}
		else
		{
			for (int j = 1; j < 4; j++)
			{
				boneType = static_cast<Leap::Bone::Type>(j);
				bone = (*fl).bone(boneType);
				Leap::Matrix bonebasis = bone.basis();

				actor->leapJointPosition[i * 4 + j] = LeapVectoPxVec3(bone.center());

				if (hand.isLeft())
					bonebasis.xBasis = -bonebasis.xBasis;

				if (j == 1)
				{
					Leap::Matrix localFingerBasis = prevBasis*bonebasis;
					PxVec3 col1, col2, col3;

					col1 = PxVec3(localFingerBasis.xBasis.x, localFingerBasis.xBasis.y, localFingerBasis.xBasis.z);
					col2 = PxVec3(localFingerBasis.yBasis.x, localFingerBasis.yBasis.y, localFingerBasis.yBasis.z);
					col3 = PxVec3(localFingerBasis.zBasis.x, localFingerBasis.zBasis.y, localFingerBasis.zBasis.z);
					PxMat33 rotatemat;
					if (handmode == HandMode::Normal || handmode == HandMode::Gogo)
						rotatemat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 });
					else if (handmode == HandMode::Mirror)
						rotatemat = PxMat33({ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 });

					PxQuat quat = PxQuat(rotatemat*PxMat33(col1, col2, col3)*rotatemat);

					actor->finger_joint[i][j]->setDrive(PxD6Drive::eSLERP, drivePos);
					actor->finger_joint[i][j]->setDrivePosition(PxTransform(quat));
				}
				else if (j == 2)
				{
					actor->finger_joint[i][j]->setDrive(PxD6Drive::eTWIST, drive[j]);
					actor->finger_joint[i][j]->setDrive(PxD6Drive::eSWING, drive[j]);
					float different = ((trackHandAngle[i][j - 1] - springHandAngle[i][j - 1]) * springTerm - dampingTerm*(w_s[i][j - 1] - w_t[i][j - 1])) / physxSpring;
					float different1 = (trackHandAbduct[i] - springHandAbduct[i]) * springTerm / physxSpring;
					PxQuat a = PxQuat(springHandAngle[i][j - 1] + different - PxHalfPi, PxVec3(1, 0, 0));
					PxQuat b = PxQuat(PxHalfPi - springHandAbduct[i] - different1, PxVec3(0, 1, 0));
					if (hand.isLeft())
						b = PxQuat(-PxHalfPi + springHandAbduct[i] + different1, PxVec3(0, 1, 0));

					if (handmode == HandMode::Mirror)
					{
						a = PxQuat(-springHandAngle[i][j - 1] - different + PxHalfPi, PxVec3(1, 0, 0));
						b = PxQuat(-PxHalfPi + springHandAbduct[i] + different1, PxVec3(0, 1, 0));
						if (hand.isLeft())
							b = PxQuat(PxHalfPi - springHandAbduct[i] - different1, PxVec3(0, 1, 0));
					}

					actor->finger_joint[i][j]->setDrivePosition(PxTransform(b*a));
				}
				else if (j > 2)
				{
					actor->finger_joint[i][j]->setDrive(PxD6Drive::eTWIST, drive[j]);
					float different = ((trackHandAngle[i][j - 1] - springHandAngle[i][j - 1]) * springTerm - dampingTerm*(w_s[i][j - 1] - w_t[i][j - 1])) / physxSpring;

					if (handmode == HandMode::Normal || handmode == HandMode::Gogo)
						actor->finger_joint[i][j]->setDrivePosition(PxTransform(PxQuat(springHandAngle[i][j - 1] + different - PxHalfPi, PxVec3(1, 0, 0))));
					else if (handmode == HandMode::Mirror)
						actor->finger_joint[i][j]->setDrivePosition(PxTransform(PxQuat(-springHandAngle[i][j - 1] - different + PxHalfPi, PxVec3(1, 0, 0))));

				}

			}
		}

		for (int j = 1; j < 4; j++)
		{
			actor->previousHandAngle[i][j - 1] = trackHandAngle[i][j - 1];
			actor->previousSpringHandAngle[i][j - 1] = springHandAngle[i][j - 1];
		}
	}

	actor->previousTime = (double)(hand.frame().timestamp() / 1000000.0);
}

void LeapClass::calculateTrackedHand(Hand hand)
{
	Leap::FingerList fingers = hand.fingers();
	int i = 0;
	for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); fl++, i++)
	{
		Leap::Bone bone_0, bone_1;
		Leap::Bone::Type boneType_0, boneType_1;
		int j = 1;
		if (i == 0)
			j++;

		for (; j < 4; j++)
		{
			boneType_0 = static_cast<Leap::Bone::Type>(j - 1);
			bone_0 = (*fl).bone(boneType_0);
			Leap::Matrix bonebasis_0 = bone_0.basis();
			float distance_0 = bone_0.length();
			boneType_1 = static_cast<Leap::Bone::Type>(j);
			bone_1 = (*fl).bone(boneType_1);
			Leap::Matrix bonebasis_1 = bone_1.basis();
			float distance_1 = bone_1.length();

			if (j == 1 || (i == 0 && j == 2))
			{
				trackHandAbduct[i] = calculateJointabductLeap(bonebasis_0, distance_0, bonebasis_1, distance_1);
			}

			trackHandAngle[i][j - 1] = calculateJointangleLeap(bonebasis_0, distance_0, bonebasis_1, distance_1);
		}
	}
}

float LeapClass::calculateJointangleLeap(Leap::Matrix basis_0, float joint_length_0, Leap::Matrix basis_1, float joint_length_1)
{
	Leap::Matrix inv = basis_0.rigidInverse();
	Leap::Matrix local = inv*basis_1;
	Leap::Vector v1 = Leap::Vector(0, local.zBasis.y, local.zBasis.z)*joint_length_1;
	Leap::Vector v0 = Leap::Vector(0, -1, 0)*joint_length_0;

	float angle = v1.angleTo(v0);
	return angle;
}

float LeapClass::calculateJointabductLeap(Leap::Matrix basis_0, float joint_length_0, Leap::Matrix basis_1, float joint_length_1)
{
	Leap::Matrix inv = basis_0.rigidInverse();
	Leap::Matrix local = inv*basis_1;
	Leap::Vector v1 = Leap::Vector(local.zBasis.x, 0, local.zBasis.z)*joint_length_1;
	Leap::Vector v0 = Leap::Vector(-1, 0, 0)*joint_length_0;

	float angle = v1.angleTo(v0);
	return angle;
}

void LeapClass::calculateSpringHand(handActor* actor)
{
	for (int i = 0; i < 5; i++)
	{
		int j = 1;
		if (i == 0)
			j++;

		for (; j < 4; j++)
		{
			//PxU32 nShapes = actor->finger_actor[i][j - 1]->getNbShapes();
			//PxShape** shapes = new PxShape*[nShapes];

			//actor->finger_actor[i][j - 1]->getShapes(shapes, nShapes);
			//PxTransform pT = PxShapeExt::getGlobalPose(*shapes[0], *actor->finger_actor[i][j - 1]);
			PxTransform pT = actor->finger_actor[i][j - 1]->getGlobalPose();
			PxQuat quat = pT.q;
			PxTransform basis_0 = PxTransform(quat);

			////PxU32 nShapes1 = actor->finger_actor[i][j]->getNbShapes();
			////PxShape** shapes1 = new PxShape*[nShapes1];

			////actor->finger_actor[i][j]->getShapes(shapes1, nShapes1);
			//pT = PxShapeExt::getGlobalPose(*shapes1[0], *actor->finger_actor[i][j]);
			pT = actor->finger_actor[i][j]->getGlobalPose();
			quat = pT.q;
			PxTransform basis_1 = PxTransform(quat);

			if (j == 1 || (i == 0 && j == 2))
			{
				springHandAbduct[i] = calculateJointabductPhysx(basis_0, actor->halfHeight[i][j - 1] * 2, basis_1, actor->halfHeight[i][j] * 2);
			}

			springHandAngle[i][j - 1] = calculateJointanglePhysx(basis_0, actor->halfHeight[i][j - 1] * 2, basis_1, actor->halfHeight[i][j] * 2);
		}
	}
}

float LeapClass::calculateJointanglePhysx(PxTransform basis_0, float joint_length_0, PxTransform basis_1, float joint_length_1)
{
	PxTransform inv = basis_0.getInverse();
	PxTransform local = inv*basis_1;
	PxQuat q = local.q;
	Leap::Vector v1 = Leap::Vector(0, q.getBasisVector2().y, q.getBasisVector2().z)*joint_length_1;
	Leap::Vector v0 = Leap::Vector(0, -1, 0)*joint_length_0;

	float angle = v1.angleTo(v0);
	return angle;
}

float LeapClass::calculateJointabductPhysx(PxTransform basis_0, float joint_length_0, PxTransform basis_1, float joint_length_1)
{
	PxTransform inv = basis_0.getInverse();
	PxTransform local = inv*basis_1;
	PxQuat q = local.q;
	Leap::Vector v1 = Leap::Vector(q.getBasisVector2().x, 0, q.getBasisVector2().z)*joint_length_1;
	Leap::Vector v0 = Leap::Vector(-1, 0, 0)*joint_length_0;

	float angle = v1.angleTo(v0);
	return angle;
}

void LeapClass::deleteHand(handActor* actor)
{
	actor->palm->release();
	actor->palm = NULL;
	for (int i = 0; i < 5; i++)
	{
		int j = 0;
		if (i == 0)
			j = 1;

		for (; j < 4; j++)
		{
			////achor[i][j]->release();
			gScene->removeActor(*actor->finger_actor[i][j]);
			actor->finger_actor[i][j]->release();
			actor->finger_joint[i][j]->release();
			
		}
	}
	if (actor->grabActor != NULL)
	{
		actor->grabActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
		actor->grabActor = NULL;
	}
	
	actor->leapJointPosition.~vector();
	//delete[] actor->fingerTipPosition;
	gScene->removeAggregate(*(actor->aggregate));
	actor->aggregate->release();
	delete actor;
}

PxRigidDynamic* LeapClass::createJoint(Vector bone, PxRigidDynamic* finger_joint, PxVec3 attachPosition)
{
	float maxDist = 0.05f;
	float damping = 10.0;
	float stifness = 10.0f;
	PxRigidDynamic* newActor = CreateSphere(leapToWorld(bone, HandMode::Normal), 0.1, 1.0f);
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

PxRigidDynamic* LeapClass::createCylinder(PxReal radius, PxReal halfHeight, PxVec3 pos, PxQuat quat, PxAggregate* aggregate)
{
	if (halfHeight == 0)
		halfHeight = 0.1;
	PxTransform transform(pos);
	PxMaterial* mMaterial = gPhysicsSDK->createMaterial(1.0, 1.0, 0.5);
	PxReal         density = 1.0f;
	PxCapsuleGeometry geometry(radius, halfHeight);
	//PxRigidDynamic *actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);
	PxRigidDynamic* actor = gPhysicsSDK->createRigidDynamic(transform);
	PxShape* capsule = actor->createShape(geometry, *mMaterial);
	capsule->setLocalPose(PxTransform(quat));
	//	actor->setLinearVelocity(PxVec3(0, 0, 0));
	actor->setSolverIterationCounts(32, 32);
	if (!actor)
		cerr << "create actor failed!" << endl;
	//aggregate->addActor(*actor);
	actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	return actor;
}

std::vector<handActor*> LeapClass::getHandActor()
{
	return mHandList;
}

std::vector<handActor*> LeapClass::getMirrorHandActor()
{
	//return mMirrorHandList;
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
	for (map<int, PxRigidDynamic*>::iterator ii = activeContact.begin(); ii != activeContact.end(); ++ii)
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
					PxVec3 handPos = leapToWorld(hand.palmPosition(), HandMode::Normal);
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
		converted.z = -hand.wristPosition().z / 100.f - 2.9f;
		return converted;
	}
	else
	{
		return PxVec3(0, 0, 0);
	}
}

void LeapClass::switchMirrorHand(int mode)
{
	if (mode == 0)
	{
		normalHand = true;
		mirrorHand = false;
	}
	else if (mode == 1)
	{
		normalHand = false;
		mirrorHand = true;
	}
	else
	{
		normalHand = true;
		mirrorHand = true;
	}
}

Leap::Vector LeapClass::PxVec3toLeapVec(PxVec3 pos)
{
	return Leap::Vector(pos.x, pos.y, pos.z);
}

PxVec3 LeapClass::LeapVectoPxVec3(Leap::Vector pos)
{
	return PxVec3(pos.x, pos.y, pos.z);
}

PxRigidDynamic* LeapClass::queryScene(PxTransform transform, PxGeometry geometry)
{
	PxOverlapBuffer hit;            // [out] Overlap results
	//PxGeometry overlapShape = PxBoxGeometry(10.0,10,10);  // [in] shape to test for overlaps
	PxTransform shapePose = transform;    // [in] initial shape pose (at distance=0)
	PxQueryFilterData filterData = PxQueryFilterData(PxQueryFlag::eDYNAMIC);
	filterData.flags |= PxQueryFlag::eANY_HIT;
	filterData.data.word0 = FilterGroup::eBox;
	bool status = gScene->overlap(geometry, shapePose, hit, filterData);
	PxRigidDynamic* hitactor;
	if (status)
	{
		hitactor = (PxRigidDynamic*)hit.block.actor;
	}
	else
		hitactor = NULL;

	return hitactor;
}

std::vector<PxRigidActor*> LeapClass::getBoxes()
{
	return boxes;
}

void LeapClass::clearBoxes()
{
	boxes.clear();
}

bool LeapClass::getRightHandMirror()
{
	return rightHandMirrored;
}

void LeapClass::setHandMode(bool gogo)
{
	if(gogo)
		globalHandMode = HandMode::Gogo;
	if(!gogo)
		globalHandMode = HandMode::Normal;
}

void LeapClass::setNoLoseGrip(bool l)
{
	nolosegripmode = l;
}
