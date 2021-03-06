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
#include <PxSimpleFactory.h>
#include <PxMat33.h>
#include <vector>

#include <map>

//#include "physxHelper.h"

using namespace std;
using namespace Leap;
using namespace physx;
using namespace DirectX;
//using namespace Eigen;

#define SWITCHTIME 0.8f
#define SWITCHRANGE 1.8f

//static const float particleSize = 0.075f;
//static const float particleRadius = 0.075f;
#ifndef HA
#define HA
const bool OFFAXIS = false;
const bool NOMIRROR = true;
#ifndef GROUP
#define GROUP
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

#endif

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
	PxRigidDynamic* finger_actor[5][4];
	PxRigidDynamic* finger_tip_actor[5];
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
	float* pinchStrength;
	float pinchDistance;
	PxVec3 palmDimension;
	PxAggregate* aggregate;

	PxRigidDynamic* grabActor;
	int grabType;
	PxTransform grabActorStartPos;
	PxTransform grabHandStartPos;
	Leap::Vector wristPosition;
	std::vector<PxVec3> leapJointPosition;
	PxVec3* fingerTipPosition;
	PxVec3* fingerTipWorldPosition;

	bool* isExtended;
	PxTransform handTransform;
	PxTransform previousTransform[5][4];
};

class LeapClass {
public:

	const bool CORRECTPERS = true;

	vector<PxRigidActor*> proxyParticleActor;
	vector<PxRigidActor*> proxyParticleJoint;

	void connect();
	void processFrame(float headPosition_x, float headPosition_y, float headPosition_z, float offset_z, XMFLOAT4X4 mView, XMFLOAT4X4 mProj, float factor);
	void processRemoteFrame(float headPosition_x, float headPosition_y, float headPosition_z, float offset_z, XMFLOAT4X4 mView, XMFLOAT4X4 mProj, float handlength, std::string remote_frame = string());
	void InitPhysx(PxPhysics* sdk, PxScene* scene);
	vector<PxRigidActor*> getProxyParticle();
	vector<PxRigidActor*> getProxyJoint();
	std::vector<handActor*> getHandActor();
	std::vector<handActor*> getRHandActor();
	std::vector<handActor*> getMirrorHandActor();
	bool captureFingerTip(float * x, float *y, float*z);
	bool capturePalm(float* z);

	void setMatrix(PxMat44 mat);

	void setProjectionMatrix(PxMat44);
	void setViewMatrix(PxMat44);
	void clearHand();
	std::map<int, PxVec3> computeForce(std::map<int, PxRigidDynamic*> activeContact);
	PxVec3 getWristPosition(int id);
	PxVec3 leapToWorld(Leap::Vector bone_center, HandMode::Enum handmode, bool remote);
	void setoffset(float leap_y_offset, float leap_z_offset, float screenWidth, float screen_height, PxVec3 remoteHead);
	void switchMirrorHand(int mode);
	std::vector<PxRigidActor*> getBoxes();
	std::vector<PxRigidActor*> deleteBoxes;
	void clearBoxes();
	bool getRightHandMirror();
	void setHandMode(bool);
	void setNoLoseGrip(bool);

private:

	float ly_offset;
	float lz_offset; 
	float m_screenWidth;
	float m_screen_height;
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
	PxVec3 r_head;

	std::vector<handActor*> mHandList;
	std::vector<handActor*> mRHandList;

	

	PxRigidDynamic* CreateSphere(const PxVec3& pos, const PxReal radius, const PxReal density);
	PxRigidDynamic* CreateBox(PxVec3 dimension, PxVec3 pose, PxQuat quat);

	void createHand(Hand hand, bool goinUp, bool isInTransit, float factor, HandMode::Enum handMode, PxRigidDynamic* grabActor,int grabType, bool remote);

	void updateHand(Hand hand, handActor* actor, HandMode::Enum handmode , bool remote);
	void deleteHand(handActor* actor);

	void updateHands(Hand hand, handActor* actor, bool remote);
	void createHands(Hand hand, bool remote);



	PxRigidDynamic* createJoint(Leap::Vector bone, PxRigidDynamic* finger_joint, PxVec3 attachPosition);
	PxRigidDynamic* createCylinder(PxReal radius, PxReal halfHeight, PxVec3 pos, PxQuat quat, PxAggregate* aggregate);
	void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
	float z_offset;

	Leap::Vector PxVec3toLeapVec(PxVec3 pos);
	PxVec3 LeapVectoPxVec3(Leap::Vector pos);
	PxRigidDynamic * queryScene(PxTransform transform, PxGeometry geometry);

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

	bool nolosegripmode;

	vector<PxRigidActor*> boxes;

	HandMode::Enum globalHandMode;
};

#endif