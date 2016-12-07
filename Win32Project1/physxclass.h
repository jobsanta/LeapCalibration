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
#include <stdio.h>
#include <iostream>
#include <map>

#include <Windows.h>
#include "leapclass.h"

using namespace physx;
using namespace std;

class PhysxClass : public PxSimulationEventCallback
{
public:

	PhysxClass();
	~PhysxClass();
	bool Initialize();
	void Render();
	void Shutdown();
	PxRigidDynamic* createBox(PxVec3 dimension, PxVec3 pose, PxQuat quat);
	PxRigidDynamic* createSphere(PxReal radius, PxVec3 pose, PxQuat quat);
	PxRigidDynamic* createCapsule(PxReal radius, PxReal halfHeight, PxVec3 pose, PxQuat quat);
	PxRigidStatic* createStaticSphere(PxReal radius, PxVec3 pose, PxQuat quat);

	PxPhysics* getPhysicsSDK();
	PxScene* getScene();


	void removeActor(PxRigidActor* actor);
	void moveWall(float z_offset);
	int getActiveHandID();
	PxRigidDynamic* getActiveActor();
	std::map<int, PxRigidDynamic*> getActiveContact();
	void applyForce(std::map<int, PxVec3>);
	void setHandActor(std::vector<handActor> HandList);

	virtual void							onTrigger(PxTriggerPair* pairs, PxU32 count) ;
	virtual void							onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {};
	virtual void							onConstraintBreak(PxConstraintInfo*, PxU32) {}
	virtual void							onWake(PxActor**, PxU32) {}
	virtual void							onSleep(PxActor**, PxU32) {}
	bool gTouchFound;
	int gTouchId;


private:

	long long milliseconds_now();
	void StepPhysx();

	long long start_time = 0;
	long long oldTimeSinceStart = 0;
	float mAccumulator = 0;

	PxPhysics*                gPhysicsSDK = NULL;
	PxDefaultErrorCallback    gDefaultErrorCallback;
	PxDefaultAllocator        gDefaultAllocatorCallback;
	PxSimulationFilterShader  gDefaultFilterShader = PxDefaultSimulationFilterShader;
	PxFoundation*             gFoundation = NULL;
	PxScene*				  gScene = NULL;
	PxReal                    myTimestep = 1.0f / 30.0f;
	PxMaterial* mMaterial;
	PxRigidStatic* moveAbleWall;
	PxRigidStatic* moveAbleFloor;
	
	std::vector<handActor> mHandList;
	PxRigidDynamic* activeActor;
	PxRigidDynamic* activeHand;
	std::map<int, PxRigidDynamic*> activeContact;

	PxVisualDebuggerConnection* theConnection;

	vector<PxRigidActor*> boxes;
	vector<PxRigidActor*> spheres;
};