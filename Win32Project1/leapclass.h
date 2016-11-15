#include "Leap.h"

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


//#include "physxHelper.h"

using namespace std;
using namespace Leap;
using namespace physx;


//static const float particleSize = 0.075f;
//static const float particleRadius = 0.075f;
#ifndef HA
#define HA
struct handActor
{
	int id;
	bool isRef;
	PxRigidDynamic* palm;
	PxRigidDynamic* finger_actor[5][3];
	PxRigidDynamic* finger_tip[5];
	PxRigidDynamic* finger_tip_achor[5];
	PxDistanceJoint* finger_tip_joint[5];
	PxD6Joint* finger_joint[5][3];
	PxReal halfHeight[5][3];
	PxReal fingerWidth[5][3];
	PxVec3 palmDimension;
} ;



class LeapClass{
public:


	vector<PxRigidActor*> proxyParticleActor;
	vector<PxRigidActor*> proxyParticleJoint;

	void connect();
	void processFrame(float x, float y , float z, float offset,float xScale, float yScale);
	void InitPhysx(PxPhysics* sdk, PxScene* scene);
	vector<PxRigidActor*> getProxyParticle();
	vector<PxRigidActor*> getProxyJoint();
	//PxRigidActor* getPalm();
	std::vector<handActor> getHandActor();
	bool captureFingerTip(float * x, float *y, float*z);
	bool capturePalm(float* z);


	void setMatrix(PxMat44 mat);

	void setProjectionMatrix(PxMat44);
	void setViewMatrix(PxMat44);
	handActor* createRefHand(PxVec3);


private:
	Controller controller;
	int64_t lastFrameID = 0;
	int64_t lastFrameID_cap = 0;
	PxPhysics* gPhysicsSDK;
	PxScene* gScene;
	PxMat44 ltoworldtransform;
	PxMat44 viewMatrix;
	PxMat44 projectionMatrix;
	Hand calibrateHand;

	float x_head, y_head, z_head;

	std::vector<handActor> mHandList;

	PxVec3 leapToWorld(Leap::Vector bone_center);
	PxRigidDynamic* CreateSphere(const PxVec3& pos, const PxReal radius, const PxReal density);
	PxRigidDynamic* CreateBox(const PxVec3& pos, const PxReal radius, const PxReal density);
	//void createJoint(Leap::Vector bone);
	void createHand(Hand hand);

	void updateHand(Hand hand, handActor actor);
	void deleteHand(handActor actor);
	PxRigidDynamic* createJoint(Leap::Vector bone, PxRigidDynamic* finger_joint, PxVec3 attachPosition);
	PxRigidDynamic* createCylinder(PxReal radius, PxReal halfHeight, PxVec3 pos);
	void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
	float z_offset;
	float x_scale;
	float y_scale;




};

#endif