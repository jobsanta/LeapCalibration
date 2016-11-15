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

#include <Windows.h>

using namespace physx;
using namespace std;

class PhysxClass //: public PxSimulationEventCallback
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
	



	PxVisualDebuggerConnection* theConnection;

	vector<PxRigidActor*> boxes;
	vector<PxRigidActor*> spheres;
};