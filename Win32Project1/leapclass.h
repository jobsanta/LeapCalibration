#include "Leap.h"
#include "d3dclass.h"
#include <iostream>
#include <Eigen/Eigen>
#include <Eigen/Dense>

//-- Physx Library
#include <PxPhysicsAPI.h>
#include <PxExtensionsAPI.h>
#include <PxDefaultErrorCallback.h>
#include <PxDefaultAllocator.h>
#include <PxDefaultSimulationFilterShader.h>
#include <PxDefaultCpuDispatcher.h>
#include <PxShapeExt.h>
#include <PxMat33.h>
#include <PxSimpleFactory.h>
#include <vector>
#include <PxVisualDebuggerExt.h>
#include <map>

//#include "physxHelper.h"

using namespace std;
using namespace Leap;
using namespace physx;
using namespace DirectX;
using namespace Eigen;


#define SWITCHTIME 1.0f
#define SWITCHRANGE 2.0f


//static const float particleSize = 0.075f;
//static const float particleRadius = 0.075f;
#ifndef HA
#define HA
const bool OFFAXIS = false;

struct HandMode
{
	enum Enum
	{
		Normal = 0,
		Mirror = 1,
		Transit = 2,
		Gogo = 3

	};
};


struct handActor
{
	int id;
	bool isRef;
	PxRigidDynamic* palm;
	//PxRigidDynamic* palmJoint[4];
	PxRigidDynamic* finger_actor[5][4];
	PxRigidDynamic* finger_joint_actor[5][4];
	PxRigidDynamic* finger_tip[5];
	PxRigidDynamic* finger_tip_achor[5];
	PxDistanceJoint* finger_tip_joint[5];
	PxD6Joint* finger_joint[5][4];
	PxReal halfHeight[5][4];
	PxReal fingerWidth[5][4];
	float previousHandAngle[5][3];
	float previousSpringHandAngle[5][3];
	double previousTime;
	bool isInMirror;
	bool isInTransit;
	bool transitComplete;
	bool toBeDelete;
	bool goingUp;
	double startTransitTime;
	double alphaValue;

	PxVec3 palmDimension;
	PxAggregate* aggregate;

	PxVec3* leapJointPosition;
	PxVec3* leapJointPosition2;
	PxVec3* fingerTipPosition;
	bool* isExtended;

	


} ;


class LeapClass {
public:

	const bool CORRECTPERS = true;

	vector<PxRigidActor*> proxyParticleActor;
	vector<PxRigidActor*> proxyParticleJoint;

	void connect();
	void processFrame(float headPosition_x, float headPosition_y, float headPosition_z, float offset_z, XMFLOAT4X4 mView, XMFLOAT4X4 mProj, float factor);
	void InitPhysx(PxPhysics* sdk, PxScene* scene);
	vector<PxRigidActor*> getProxyParticle();
	vector<PxRigidActor*> getProxyJoint();
	//PxRigidActor* getPalm();
	std::vector<handActor*> getHandActor();
	std::vector<handActor*> getMirrorHandActor();
	bool captureFingerTip(float * x, float *y, float*z);
	bool capturePalm(float* z);


	void setMatrix(PxMat44 mat);

	void setProjectionMatrix(PxMat44);
	void setViewMatrix(PxMat44);
	void clearHand();
	std::map<int, PxVec3> computeForce(std::map<int, PxRigidDynamic*> activeContact);
	PxVec3 getWristPosition(int id);
	PxVec3 leapToWorld(Leap::Vector bone_center, HandMode::Enum handmode);
	void switchMirrorHand(int mode);
	std::vector<PxRigidActor*> getBoxes();
	std::vector<PxRigidActor*> deleteBoxes;
	void clearBoxes();
	bool getRightHandMirror();
	void setHandMode();

private:
	Controller controller;
	int64_t lastFrameID = 0;
	int64_t lastFrameID_cap = 0;
	PxPhysics* gPhysicsSDK;
	PxScene* gScene;
	PxMat44 ltoworldtransform;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	Hand calibrateHand;
	bool mirrorHand;
	bool normalHand;

	float x_head, y_head, z_head;

	std::vector<handActor*> mHandList;
//	std::vector<handActor> mMirrorHandList;


	PxRigidDynamic* CreateSphere(const PxVec3& pos, const PxReal radius, const PxReal density);
	PxRigidDynamic* CreateBox(PxVec3 dimension, PxVec3 pose, PxQuat quat);
	//void createJoint(Leap::Vector bone);
	void createHand(Hand hand, bool goinUp, bool isInTransit,float factor, HandMode::Enum handMode);

	void updateHand(Hand hand, handActor* actor, HandMode::Enum handmode);
	void deleteHand(handActor* actor);

	void updateHands(Hand hand, handActor* actor);
	void createHands(Hand hand);

	PxRigidDynamic* createJoint(Leap::Vector bone, PxRigidDynamic* finger_joint, PxVec3 attachPosition);
	PxRigidDynamic* createCylinder(PxReal radius, PxReal halfHeight, PxVec3 pos,PxQuat quat, PxAggregate* aggregate);
	void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
	float z_offset;

	Leap::Vector PxVec3toLeapVec(PxVec3 pos);
	PxVec3 LeapVectoPxVec3(Leap::Vector pos);
	//float x_scale;
	//float y_scale;

	void calculateTrackedHand(Hand hand);
	void calculateSpringHand(handActor* hand);
	float calculateJointangleLeap(Leap::Matrix basis_0, float joint_length_0, Leap::Matrix basis_1, float joint_length_1);
	float calculateJointanglePhysx(PxTransform basis_0, float joint_length_0, PxTransform basis_1, float joint_length_1);
	float calculateJointabductLeap(Leap::Matrix basis_0, float joint_length_0, Leap::Matrix basis_1, float joint_length_1);
	float calculateJointabductPhysx(PxTransform basis_0, float joint_length_0, PxTransform basis_1, float joint_length_1);
	float trackHandAngle[5][3];
	float trackHandAbduct[5];
	float springHandAngle[5][3];
	float springHandAbduct[5];
	bool computeHand;

	int numberofTransitHand;

	bool leftHandMirrored;
	bool rightHandMirrored;

	PxRigidDynamic*  queryScene(PxTransform t);
	vector<PxRigidActor*> boxes;

	HandMode::Enum globalHandMode;
	


};

#endif