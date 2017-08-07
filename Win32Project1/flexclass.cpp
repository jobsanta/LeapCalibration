#include "flex.h"
FlexClass::FlexClass()
{
	mRadius = 0.1f;
	library = NULL;
	solver = NULL;
	//particleBuffer = NULL;
	//velocityBuffer = NULL;
	//phaseBuffer = NULL;
	
}

FlexClass::~FlexClass()
{

}


void FlexClass::initialize()
{
	library = NvFlexInit();

	// alloc buffers
	g_buffers = AllocBuffers(library);

	// map during initialization
	MapBuffers(g_buffers);

	g_buffers->positions.resize(0);
	g_buffers->velocities.resize(0);
	g_buffers->phases.resize(0);

	g_buffers->rigidOffsets.resize(0);
	g_buffers->rigidIndices.resize(0);
	g_buffers->rigidMeshSize.resize(0);
	g_buffers->rigidRotations.resize(0);
	g_buffers->rigidTranslations.resize(0);
	g_buffers->rigidCoefficients.resize(0);
	g_buffers->rigidPlasticThresholds.resize(0);
	g_buffers->rigidPlasticCreeps.resize(0);
	g_buffers->rigidLocalPositions.resize(0);
	g_buffers->rigidLocalNormals.resize(0);

	g_buffers->springIndices.resize(0);
	g_buffers->springLengths.resize(0);
	g_buffers->springStiffness.resize(0);
	g_buffers->triangles.resize(0);
	g_buffers->triangleNormals.resize(0);
	g_buffers->uvs.resize(0);

	g_buffers->shapeGeometry.resize(0);
	g_buffers->shapePositions.resize(0);
	g_buffers->shapeRotations.resize(0);
	g_buffers->shapePrevPositions.resize(0);
	g_buffers->shapePrevRotations.resize(0);
	g_buffers->shapeFlags.resize(0);


	// sim params
	g_params.gravity[0] = 0.0f;
	g_params.gravity[1] = -9.8f;
	g_params.gravity[2] = 0.0f;

	g_params.wind[0] = 0.0f;
	g_params.wind[1] = 0.0f;
	g_params.wind[2] = 0.0f;

	g_params.radius = 0.15f;
	g_params.viscosity = 0.0f;
	g_params.dynamicFriction = 0.0f;
	g_params.staticFriction = 0.0f;
	g_params.particleFriction = 0.0f; // scale friction between particles by default
	g_params.freeSurfaceDrag = 0.0f;
	g_params.drag = 0.0f;
	g_params.lift = 0.0f;
	g_params.numIterations = 3;
	g_params.fluidRestDistance = 0.0f;
	g_params.solidRestDistance = 0.0f;

	g_params.anisotropyScale = 1.0f;
	g_params.anisotropyMin = 0.1f;
	g_params.anisotropyMax = 2.0f;
	g_params.smoothing = 1.0f;

	g_params.dissipation = 0.0f;
	g_params.damping = 0.0f;
	g_params.particleCollisionMargin = 0.0f;
	g_params.shapeCollisionMargin = 0.0f;
	g_params.collisionDistance = 0.0f;
	g_params.sleepThreshold = 0.0f;
	g_params.shockPropagation = 0.0f;
	g_params.restitution = 0.0f;

	g_params.maxSpeed = FLT_MAX;
	g_params.maxAcceleration = 100.0f;	// approximately 10x gravity

	g_params.relaxationMode = eNvFlexRelaxationLocal;
	g_params.relaxationFactor = 1.0f;
	g_params.solidPressure = 1.0f;
	g_params.adhesion = 0.0f;
	g_params.cohesion = 0.025f;
	g_params.surfaceTension = 0.0f;
	g_params.vorticityConfinement = 0.0f;
	g_params.buoyancy = 1.0f;
	g_params.diffuseThreshold = 100.0f;
	g_params.diffuseBuoyancy = 1.0f;
	g_params.diffuseDrag = 0.8f;
	g_params.diffuseBallistic = 16;
	g_params.diffuseLifetime = 2.0f;


	// planes created after particles
	g_params.numPlanes = 1;


	// create new solver
	NvFlexSolverDesc solverDesc;
	NvFlexSetSolverDescDefaults(&solverDesc);

	// -----------To-do------------
	//Config Scene here
	Instance octo("../Win32Project1/data/octopus.obj");
	octo.mScale = Vec3(32);
	octo.mRadius = 0.05f;
	octo.mClusterSpacing = 1.5f;
	octo.mClusterRadius = 0.0f;
	octo.mClusterStiffness = 0.1f;
	octo.mGlobalStiffness = 1.0f;
	octo.mLinkStiffness = 1.0f;
	octo.mClusterPlasticThreshold = 0.0015f;
	octo.mClusterPlasticCreep = 0.25f;
	octo.mTranslation[0] = -1;
	octo.mTranslation[1] = 1.0f;
	octo.mTranslation[2] = -4;

	mInstances.push_back(octo);

	Instance bowl("../Win32Project1/data/bowl_high.ply");
	bowl.mScale = Vec3(20.0);
	bowl.mClusterSpacing = 10.0f;
	bowl.mClusterRadius = 10.0f;
	bowl.mClusterStiffness = 0.1;
	bowl.mVolumeSampling = 2.0f;
	bowl.mRadius = 0.05f;
	bowl.mTranslation[1] = 3.0f;
	bowl.mTranslation[2] = -3.0f;
	mInstances.push_back(bowl);

	g_buffers->rigidOffsets.push_back(0);

	mRenderingInstances.resize(0);

	// build soft bodies 
	for (int i = 0; i < int(mInstances.size()); i++)
		CreateSoftBody(mInstances[i], mRenderingInstances.size());

	// fix any particles below the ground plane in place
	//for (int i = 0; i < int(g_buffers->positions.size()); ++i)
	//	if (g_buffers->positions[i].y < 0.0f)
	//		g_buffers->positions[i].w = 0.0f;
	Quat rot = QuatFromAxisAngle(Vec3(0.0f, 1.0f, 0.0f), 0.0);
	//AddSphere(1.0, Vec3(0.5, 1, 1), rot);
	//g_params.radius *= 1.5f;

	uint32_t numParticles = g_buffers->positions.size();
	uint32_t maxParticles = numParticles + g_numExtraParticles*g_numExtraMultiplier;
	if (g_params.solidRestDistance == 0.0f)
		g_params.solidRestDistance = g_params.radius;

	// if fluid present then we assume solid particles have the same radius
	if (g_params.fluidRestDistance > 0.0f)
		g_params.solidRestDistance = g_params.fluidRestDistance;

	// set collision distance automatically based on rest distance if not alraedy set
	if (g_params.collisionDistance == 0.0f)
		g_params.collisionDistance = Max(g_params.solidRestDistance, g_params.fluidRestDistance)*0.5f;

	// default particle friction to 10% of shape friction
	if (g_params.particleFriction == 0.0f)
		g_params.particleFriction = g_params.dynamicFriction*0.1f;

	// add a margin for detecting contacts between particles and shapes
	if (g_params.shapeCollisionMargin == 0.0f)
		g_params.shapeCollisionMargin = g_params.collisionDistance*0.5f;


	Vec3 up = Normalize(Vec3(0, 1.0f, 0.0f));

	(Vec4&)g_params.planes[0] = Vec4(up.x, up.y, up.z, -1.0f);
	(Vec4&)g_params.planes[1] = Vec4(0.0f, 0.0f, 1.0f, -3);
	(Vec4&)g_params.planes[2] = Vec4(1.0f, 0.0f, 0.0f, -3);
	(Vec4&)g_params.planes[3] = Vec4(-1.0f, 0.0f, 0.0f, 3);
	(Vec4&)g_params.planes[4] = Vec4(0.0f, 0.0f, -1.0f, 3);
	(Vec4&)g_params.planes[5] = Vec4(0.0f, -1.0f, 0.0f, 5);

	g_buffers->diffusePositions.resize(0);
	g_buffers->diffuseVelocities.resize(0);
	g_buffers->diffuseCount.resize(1, 0);

	// for fluid rendering these are the Laplacian smoothed positions
	g_buffers->smoothPositions.resize(maxParticles);

	g_buffers->normals.resize(0);
	g_buffers->normals.resize(maxParticles);

	// initialize normals (just for rendering before simulation starts)
	int numTris = g_buffers->triangles.size() / 3;
	for (int i = 0; i < numTris; ++i)
	{
		Vec3 v0 = Vec3(g_buffers->positions[g_buffers->triangles[i * 3 + 0]]);
		Vec3 v1 = Vec3(g_buffers->positions[g_buffers->triangles[i * 3 + 1]]);
		Vec3 v2 = Vec3(g_buffers->positions[g_buffers->triangles[i * 3 + 2]]);

		Vec3 n = Cross(v1 - v0, v2 - v0);

		g_buffers->normals[g_buffers->triangles[i * 3 + 0]] += Vec4(n, 0.0f);
		g_buffers->normals[g_buffers->triangles[i * 3 + 1]] += Vec4(n, 0.0f);
		g_buffers->normals[g_buffers->triangles[i * 3 + 2]] += Vec4(n, 0.0f);
	}

	for (int i = 0; i < int(maxParticles); ++i)
		g_buffers->normals[i] = Vec4(SafeNormalize(Vec3(g_buffers->normals[i]), Vec3(0.0f, 1.0f, 0.0f)), 0.0f);



	solverDesc.maxParticles = maxParticles;
	solverDesc.maxDiffuseParticles = 0;
	solverDesc.maxNeighborsPerParticle = 96;
	// no fluids or sdf based collision
	solverDesc.featureMode = eNvFlexFeatureModeSimpleSolids;


	// main create method for the Flex solver
	solver = NvFlexCreateSolver(library, &solverDesc);
	
	// create active indices (just a contiguous block for the demo)
	g_buffers->activeIndices.resize(g_buffers->positions.size());
	for (int i = 0; i < g_buffers->activeIndices.size(); ++i)
		g_buffers->activeIndices[i] = i;

	// resize particle buffers to fit
	g_buffers->positions.resize(maxParticles);
	g_buffers->velocities.resize(maxParticles);
	g_buffers->phases.resize(maxParticles);

	g_buffers->densities.resize(maxParticles);
	g_buffers->anisotropy1.resize(maxParticles);
	g_buffers->anisotropy2.resize(maxParticles);
	g_buffers->anisotropy3.resize(maxParticles);

	// save rest positions
	g_buffers->restPositions.resize(g_buffers->positions.size());
	for (int i = 0; i < g_buffers->positions.size(); ++i)
		g_buffers->restPositions[i] = g_buffers->positions[i];


	//// builds rigids constraints
	if (g_buffers->rigidOffsets.size())
	{
		assert(g_buffers->rigidOffsets.size() > 1);

		const int numRigids = g_buffers->rigidOffsets.size() - 1;

		// If the centers of mass for the rigids are not yet computed, this is done here
		// (If the CreateParticleShape method is used instead of the NvFlexExt methods, the centers of mass will be calculated here)
		if (g_buffers->rigidTranslations.size() == 0)
		{
			g_buffers->rigidTranslations.resize(g_buffers->rigidOffsets.size() - 1, Vec3());
			//CalculateRigidCentersOfMass(&g_buffers->positions[0], g_buffers->positions.size(), &g_buffers->rigidOffsets[0], &g_buffers->rigidTranslations[0], &g_buffers->rigidIndices[0], numRigids);
		}

		// calculate local rest space positions
		g_buffers->rigidLocalPositions.resize(g_buffers->rigidOffsets.back());
		CalculateRigidLocalPositions(&g_buffers->positions[0], &g_buffers->rigidOffsets[0], &g_buffers->rigidTranslations[0], &g_buffers->rigidIndices[0], numRigids, &g_buffers->rigidLocalPositions[0]);

		// set rigidRotations to correct length, probably NULL up until here
		g_buffers->rigidRotations.resize(g_buffers->rigidOffsets.size() - 1, Quat());
	}

	// unmap so we can start transferring data to GPU
	UnmapBuffers(g_buffers);

	//-----------------------------
	// Send data to Flex

	NvFlexCopyDesc copyDesc;
	copyDesc.dstOffset = 0;
	copyDesc.srcOffset = 0;
	copyDesc.elementCount = numParticles;

	NvFlexSetParams(solver, &g_params);
	NvFlexSetParticles(solver, g_buffers->positions.buffer, &copyDesc);
	NvFlexSetVelocities(solver, g_buffers->velocities.buffer, &copyDesc);
	NvFlexSetNormals(solver, g_buffers->normals.buffer, &copyDesc);
	NvFlexSetPhases(solver, g_buffers->phases.buffer, &copyDesc);
	NvFlexSetRestParticles(solver, g_buffers->restPositions.buffer, &copyDesc);

	NvFlexSetActive(solver, g_buffers->activeIndices.buffer, &copyDesc);
	NvFlexSetActiveCount(solver, numParticles);

	// springs
	if (g_buffers->springIndices.size())
	{
		assert((g_buffers->springIndices.size() & 1) == 0);
		assert((g_buffers->springIndices.size() / 2) == g_buffers->springLengths.size());

		NvFlexSetSprings(solver, g_buffers->springIndices.buffer, g_buffers->springLengths.buffer, g_buffers->springStiffness.buffer, g_buffers->springLengths.size());
	}

	// rigids
	if (g_buffers->rigidOffsets.size())
	{
		NvFlexSetRigids(solver, g_buffers->rigidOffsets.buffer, g_buffers->rigidIndices.buffer, g_buffers->rigidLocalPositions.buffer, g_buffers->rigidLocalNormals.buffer, g_buffers->rigidCoefficients.buffer, g_buffers->rigidPlasticThresholds.buffer, g_buffers->rigidPlasticCreeps.buffer, g_buffers->rigidRotations.buffer, g_buffers->rigidTranslations.buffer, g_buffers->rigidOffsets.size() - 1, g_buffers->rigidIndices.size());
	}

	// inflatables
	if (g_buffers->inflatableTriOffsets.size())
	{
		NvFlexSetInflatables(solver, g_buffers->inflatableTriOffsets.buffer, g_buffers->inflatableTriCounts.buffer, g_buffers->inflatableVolumes.buffer, g_buffers->inflatablePressures.buffer, g_buffers->inflatableCoefficients.buffer, g_buffers->inflatableTriOffsets.size());
	}

	// dynamic triangles
	if (g_buffers->triangles.size())
	{
		NvFlexSetDynamicTriangles(solver, g_buffers->triangles.buffer, g_buffers->triangleNormals.buffer, g_buffers->triangles.size() / 3);
	}

	// collision shapes
	if (g_buffers->shapeFlags.size())
	{
		NvFlexSetShapes(
			solver,
			g_buffers->shapeGeometry.buffer,
			g_buffers->shapePositions.buffer,
			g_buffers->shapeRotations.buffer,
			g_buffers->shapePrevPositions.buffer,
			g_buffers->shapePrevRotations.buffer,
			g_buffers->shapeFlags.buffer,
			int(g_buffers->shapeFlags.size()));
	}



}

void FlexClass::preupdate()
{
	MapBuffers(g_buffers);
}

void FlexClass::AddSphere(float radius, Vec3 position, Quat rotation)
{
	g_shapesChanged = true;
	NvFlexCollisionGeometry geo;
	geo.sphere.radius = radius;
	g_buffers->shapeGeometry.push_back(geo);

	g_buffers->shapePositions.push_back(Vec4(position, 0.0f));
	g_buffers->shapeRotations.push_back(rotation);

	g_buffers->shapePrevPositions.push_back(g_buffers->shapePositions.back());
	g_buffers->shapePrevRotations.push_back(g_buffers->shapeRotations.back());

	int flags = NvFlexMakeShapeFlags(eNvFlexShapeSphere, false);
	g_buffers->shapeFlags.push_back(flags);
}

void FlexClass::update()
{
	//Update Particle Position
	//Render Here
	particle.resize(g_buffers->positions.size());
	for (int i = 0; i < g_buffers->positions.size(); ++i)
		particle[i] = g_buffers->positions[i];

	UnmapBuffers(g_buffers);
	// send any particle updates to the solver
	NvFlexSetParticles(solver, g_buffers->positions.buffer, NULL);
	NvFlexSetVelocities(solver, g_buffers->velocities.buffer, NULL);
	NvFlexSetPhases(solver, g_buffers->phases.buffer, NULL);
	NvFlexSetActive(solver, g_buffers->activeIndices.buffer, NULL);

	NvFlexSetActiveCount(solver, g_buffers->activeIndices.size());

	// allow scene to update constraints etc
	//SyncScene();
	if (g_shapesChanged)
	{
		NvFlexSetShapes(
			solver,
			g_buffers->shapeGeometry.buffer,
			g_buffers->shapePositions.buffer,
			g_buffers->shapeRotations.buffer,
			g_buffers->shapePrevPositions.buffer,
			g_buffers->shapePrevRotations.buffer,
			g_buffers->shapeFlags.buffer,
			int(g_buffers->shapeFlags.size()));
		g_shapesChanged = false;
	
	}


		// tick solver
	NvFlexSetParams(solver, &g_params);
	NvFlexUpdateSolver(solver, 1/60.0f, 4, false);


	// read back base particle data
	// Note that flexGet calls don't wait for the GPU, they just queue a GPU copy 
	// to be executed later.
	// When we're ready to read the fetched buffers we'll Map them, and that's when
	// the CPU will wait for the GPU flex update and GPU copy to finish.
	NvFlexGetParticles(solver, g_buffers->positions.buffer, NULL);
	NvFlexGetVelocities(solver, g_buffers->velocities.buffer, NULL);
	NvFlexGetNormals(solver, g_buffers->normals.buffer, NULL);

	// readback triangle normals
	if (g_buffers->triangles.size())
		NvFlexGetDynamicTriangles(solver, g_buffers->triangles.buffer, g_buffers->triangleNormals.buffer, g_buffers->triangles.size() / 3);

	// readback rigid transforms
	if (g_buffers->rigidOffsets.size())
		NvFlexGetRigids(solver, NULL, NULL, NULL, NULL, NULL, NULL, NULL, g_buffers->rigidRotations.buffer, g_buffers->rigidTranslations.buffer);


}

void FlexClass::shutdown()
{
	// free buffers
	DestroyBuffers(g_buffers);

	NvFlexDestroySolver(solver);
	NvFlexShutdown(library);
}

void FlexClass::MapBuffers(SimBuffers* buffers)
{
	buffers->positions.map();
	buffers->restPositions.map();
	buffers->velocities.map();
	buffers->phases.map();
	buffers->densities.map();
	buffers->anisotropy1.map();
	buffers->anisotropy2.map();
	buffers->anisotropy3.map();
	buffers->normals.map();
	buffers->diffusePositions.map();
	buffers->diffuseVelocities.map();
	buffers->diffuseCount.map();
	buffers->smoothPositions.map();
	buffers->activeIndices.map();

	// convexes
	buffers->shapeGeometry.map();
	buffers->shapePositions.map();
	buffers->shapeRotations.map();
	buffers->shapePrevPositions.map();
	buffers->shapePrevRotations.map();
	buffers->shapeFlags.map();

	buffers->rigidOffsets.map();
	buffers->rigidIndices.map();
	buffers->rigidMeshSize.map();
	buffers->rigidCoefficients.map();
	buffers->rigidPlasticThresholds.map();
	buffers->rigidPlasticCreeps.map();
	buffers->rigidRotations.map();
	buffers->rigidTranslations.map();
	buffers->rigidLocalPositions.map();
	buffers->rigidLocalNormals.map();

	buffers->springIndices.map();
	buffers->springLengths.map();
	buffers->springStiffness.map();

	// inflatables
	buffers->inflatableTriOffsets.map();
	buffers->inflatableTriCounts.map();
	buffers->inflatableVolumes.map();
	buffers->inflatableCoefficients.map();
	buffers->inflatablePressures.map();

	buffers->triangles.map();
	buffers->triangleNormals.map();
	buffers->uvs.map();
}

void FlexClass::UnmapBuffers(SimBuffers* buffers)
{
	// particles
	buffers->positions.unmap();
	buffers->restPositions.unmap();
	buffers->velocities.unmap();
	buffers->phases.unmap();
	buffers->densities.unmap();
	buffers->anisotropy1.unmap();
	buffers->anisotropy2.unmap();
	buffers->anisotropy3.unmap();
	buffers->normals.unmap();
	buffers->diffusePositions.unmap();
	buffers->diffuseVelocities.unmap();
	buffers->diffuseCount.unmap();
	buffers->smoothPositions.unmap();
	buffers->activeIndices.unmap();

	// convexes
	buffers->shapeGeometry.unmap();
	buffers->shapePositions.unmap();
	buffers->shapeRotations.unmap();
	buffers->shapePrevPositions.unmap();
	buffers->shapePrevRotations.unmap();
	buffers->shapeFlags.unmap();

	// rigids
	buffers->rigidOffsets.unmap();
	buffers->rigidIndices.unmap();
	buffers->rigidMeshSize.unmap();
	buffers->rigidCoefficients.unmap();
	buffers->rigidPlasticThresholds.unmap();
	buffers->rigidPlasticCreeps.unmap();
	buffers->rigidRotations.unmap();
	buffers->rigidTranslations.unmap();
	buffers->rigidLocalPositions.unmap();
	buffers->rigidLocalNormals.unmap();

	// springs
	buffers->springIndices.unmap();
	buffers->springLengths.unmap();
	buffers->springStiffness.unmap();

	// inflatables
	buffers->inflatableTriOffsets.unmap();
	buffers->inflatableTriCounts.unmap();
	buffers->inflatableVolumes.unmap();
	buffers->inflatableCoefficients.unmap();
	buffers->inflatablePressures.unmap();

	// triangles
	buffers->triangles.unmap();
	buffers->triangleNormals.unmap();
	buffers->uvs.unmap();

}

SimBuffers* FlexClass::AllocBuffers(NvFlexLibrary* lib)
{
	return new SimBuffers(lib);
}

void FlexClass::DestroyBuffers(SimBuffers* buffers)
{
	// particles
	buffers->positions.destroy();
	buffers->restPositions.destroy();
	buffers->velocities.destroy();
	buffers->phases.destroy();
	buffers->densities.destroy();
	buffers->anisotropy1.destroy();
	buffers->anisotropy2.destroy();
	buffers->anisotropy3.destroy();
	buffers->normals.destroy();
	buffers->diffusePositions.destroy();
	buffers->diffuseVelocities.destroy();
	buffers->diffuseCount.destroy();
	buffers->smoothPositions.destroy();
	buffers->activeIndices.destroy();

	// convexes
	buffers->shapeGeometry.destroy();
	buffers->shapePositions.destroy();
	buffers->shapeRotations.destroy();
	buffers->shapePrevPositions.destroy();
	buffers->shapePrevRotations.destroy();
	buffers->shapeFlags.destroy();

	// rigids
	buffers->rigidOffsets.destroy();
	buffers->rigidIndices.destroy();
	buffers->rigidMeshSize.destroy();
	buffers->rigidCoefficients.destroy();
	buffers->rigidPlasticThresholds.destroy();
	buffers->rigidPlasticCreeps.destroy();
	buffers->rigidRotations.destroy();
	buffers->rigidTranslations.destroy();
	buffers->rigidLocalPositions.destroy();
	buffers->rigidLocalNormals.destroy();

	// springs
	buffers->springIndices.destroy();
	buffers->springLengths.destroy();
	buffers->springStiffness.destroy();

	// inflatables
	buffers->inflatableTriOffsets.destroy();
	buffers->inflatableTriCounts.destroy();
	buffers->inflatableVolumes.destroy();
	buffers->inflatableCoefficients.destroy();
	buffers->inflatablePressures.destroy();

	// triangles
	buffers->triangles.destroy();
	buffers->triangleNormals.destroy();
	buffers->uvs.destroy();

	delete buffers;
}

void FlexClass::CreateSoftBody(Instance instance, int group)
{
	plasticDeformation = false;
	float radius = 0.1f;


	g_params.radius = radius;
	g_params.dynamicFriction = 1.0f;
	g_params.particleFriction = 1.0f;
	g_params.numIterations = 4;
	g_params.collisionDistance = mRadius*0.75f;

	g_params.relaxationFactor = 1.0;


	RenderingInstance renderingInstance;

	Mesh* mesh = ImportMesh(instance.mFile);
	mesh->Normalize();
	mesh->Transform(TranslationMatrix(Point3(instance.mTranslation))*ScaleMatrix(instance.mScale*instance.mRadius));

	renderingInstance.mMesh = mesh;
	renderingInstance.mColor = instance.mColor;
	renderingInstance.mOffset = g_buffers->rigidTranslations.size();


	// create soft body definition
	NvFlexExtAsset* asset = NvFlexExtCreateSoftFromMesh(
		(float*)&renderingInstance.mMesh->m_positions[0],
		renderingInstance.mMesh->m_positions.size(),
		(int*)&renderingInstance.mMesh->m_indices[0],
		renderingInstance.mMesh->m_indices.size(),
		instance.mRadius,
		instance.mVolumeSampling,
		instance.mSurfaceSampling,
		instance.mClusterSpacing*instance.mRadius,
		instance.mClusterRadius*instance.mRadius,
		instance.mClusterStiffness,
		instance.mLinkRadius*instance.mRadius,
		instance.mLinkStiffness,
		instance.mGlobalStiffness,
		instance.mClusterPlasticThreshold,
		instance.mClusterPlasticCreep);


	// create skinning
	const int maxWeights = 4;

	renderingInstance.mSkinningIndices.resize(renderingInstance.mMesh->m_positions.size()*maxWeights);
	renderingInstance.mSkinningWeights.resize(renderingInstance.mMesh->m_positions.size()*maxWeights);

	for (int i = 0; i < asset->numShapes; ++i)
		renderingInstance.mRigidRestPoses.push_back(Vec3(&asset->shapeCenters[i * 3]));


	NvFlexExtCreateSoftMeshSkinning(
		(float*)&renderingInstance.mMesh->m_positions[0],
		renderingInstance.mMesh->m_positions.size(),
		asset->shapeCenters,
		asset->numShapes,
		instance.mSkinningFalloff,
		instance.mSkinningMaxDistance,
		&renderingInstance.mSkinningWeights[0],
		&renderingInstance.mSkinningIndices[0]);


	const int particleOffset = g_buffers->positions.size();
	const int indexOffset = g_buffers->rigidOffsets.back();

	// add particle data to solver
	for (int i = 0; i < asset->numParticles; ++i)
	{
		g_buffers->positions.push_back(&asset->particles[i * 4]);
		g_buffers->velocities.push_back(0.0f);

		const int phase = NvFlexMakePhase(group, eNvFlexPhaseSelfCollide | eNvFlexPhaseSelfCollideFilter);
		g_buffers->phases.push_back(phase);
	}

	// add shape data to solver
	for (int i = 0; i < asset->numShapeIndices; ++i)
		g_buffers->rigidIndices.push_back(asset->shapeIndices[i] + particleOffset);

	for (int i = 0; i < asset->numShapes; ++i)
	{
		g_buffers->rigidOffsets.push_back(asset->shapeOffsets[i] + indexOffset);
		g_buffers->rigidTranslations.push_back(Vec3(&asset->shapeCenters[i * 3]));
		g_buffers->rigidRotations.push_back(Quat());
		g_buffers->rigidCoefficients.push_back(asset->shapeCoefficients[i]);
	}


	// add plastic deformation data to solver, if at least one asset has non-zero plastic deformation coefficients, leave the according pointers at NULL otherwise
	if (plasticDeformation)
	{
		if (asset->shapePlasticThresholds && asset->shapePlasticCreeps)
		{
			for (int i = 0; i < asset->numShapes; ++i)
			{
				g_buffers->rigidPlasticThresholds.push_back(asset->shapePlasticThresholds[i]);
				g_buffers->rigidPlasticCreeps.push_back(asset->shapePlasticCreeps[i]);
			}
		}
		else
		{
			for (int i = 0; i < asset->numShapes; ++i)
			{
				g_buffers->rigidPlasticThresholds.push_back(0.0f);
				g_buffers->rigidPlasticCreeps.push_back(0.0f);
			}
		}
	}
	else
	{
		if (asset->shapePlasticThresholds && asset->shapePlasticCreeps)
		{
			int oldBufferSize = g_buffers->rigidCoefficients.size() - asset->numShapes;

			g_buffers->rigidPlasticThresholds.resize(oldBufferSize);
			g_buffers->rigidPlasticCreeps.resize(oldBufferSize);

			for (int i = 0; i < oldBufferSize; i++)
			{
				g_buffers->rigidPlasticThresholds[i] = 0.0f;
				g_buffers->rigidPlasticCreeps[i] = 0.0f;
			}

			for (int i = 0; i < asset->numShapes; ++i)
			{
				g_buffers->rigidPlasticThresholds.push_back(asset->shapePlasticThresholds[i]);
				g_buffers->rigidPlasticCreeps.push_back(asset->shapePlasticCreeps[i]);
			}

			plasticDeformation = true;
		}
	}

	// add link data to the solver 
	for (int i = 0; i < asset->numSprings; ++i)
	{
		g_buffers->springIndices.push_back(asset->springIndices[i * 2 + 0]);
		g_buffers->springIndices.push_back(asset->springIndices[i * 2 + 1]);

		g_buffers->springStiffness.push_back(asset->springCoefficients[i]);
		g_buffers->springLengths.push_back(asset->springRestLengths[i]);
	}

	NvFlexExtDestroyAsset(asset);

	mRenderingInstances.push_back(renderingInstance);
}

void FlexClass::CalculateRigidLocalPositions(const Vec4* restPositions, const int* offsets, const Vec3* translations, const int* indices, int numRigids, Vec3* localPositions)
{
	int count = 0;

	for (int i = 0; i < numRigids; ++i)
	{
		const int startIndex = offsets[i];
		const int endIndex = offsets[i + 1];

		assert(endIndex - startIndex);

		for (int j = startIndex; j < endIndex; ++j)
		{
			const int r = indices[j];

			localPositions[count++] = Vec3(restPositions[r]) - translations[i];
		}
	}
}

void FlexClass::ClearShapes()
{
	g_shapesChanged = true;
	g_buffers->shapeGeometry.resize(0);
	g_buffers->shapePositions.resize(0);
	g_buffers->shapeRotations.resize(0);
	g_buffers->shapePrevPositions.resize(0);
	g_buffers->shapePrevRotations.resize(0);
	g_buffers->shapeFlags.resize(0);
}

void FlexClass::AddCapsule(float radius, float halfHeight, Vec3 position, Quat rotation, Vec3 prevposition, Quat prevrotation)
{
	NvFlexCollisionGeometry geo;
	geo.capsule.radius = radius;
	geo.capsule.halfHeight = halfHeight;

	g_buffers->shapeGeometry.push_back(geo);

	g_buffers->shapePositions.push_back(Vec4(position, 0.0f));
	g_buffers->shapeRotations.push_back(rotation);

	g_buffers->shapePrevPositions.push_back(Vec4(prevposition, 0.0f));
	g_buffers->shapePrevRotations.push_back(prevrotation);

	int flags = NvFlexMakeShapeFlags(eNvFlexShapeCapsule, false);
	g_buffers->shapeFlags.push_back(flags);
}


void FlexClass::AddBox(Vec3 halfEdge, Vec3 center, Quat quat, Vec3 prevposition, Quat prevrotation, bool dynamic, int channels)
{
	// transform
	g_buffers->shapePositions.push_back(Vec4(center.x, center.y, center.z, 0.0f));
	g_buffers->shapeRotations.push_back(quat);

	g_buffers->shapePrevPositions.push_back(Vec4(prevposition, 0.0f));
	g_buffers->shapePrevRotations.push_back(prevrotation);
	NvFlexCollisionGeometry geo;
	geo.box.halfExtents[0] = halfEdge.x;
	geo.box.halfExtents[1] = halfEdge.y;
	geo.box.halfExtents[2] = halfEdge.z;

	g_buffers->shapeGeometry.push_back(geo);
	g_buffers->shapeFlags.push_back(NvFlexMakeShapeFlagsWithChannels(eNvFlexShapeBox, dynamic, channels));
}
