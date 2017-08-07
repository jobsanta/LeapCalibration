#include <NvFlex.h>
#include <NvFlexDevice.h>
#include <NvFlexExt.h>
#include <mesh.h>


struct SimBuffers
{

	NvFlexVector<Vec4> positions;
	NvFlexVector<Vec4> restPositions;
	NvFlexVector<Vec3> velocities;
	NvFlexVector<int> phases;
	NvFlexVector<float> densities;
	NvFlexVector<Vec4> anisotropy1;
	NvFlexVector<Vec4> anisotropy2;
	NvFlexVector<Vec4> anisotropy3;
	NvFlexVector<Vec4> normals;
	NvFlexVector<Vec4> smoothPositions;
	NvFlexVector<Vec4> diffusePositions;
	NvFlexVector<Vec4> diffuseVelocities;
	NvFlexVector<int> diffuseCount;

	NvFlexVector<int> activeIndices;

	// convexes
	NvFlexVector<NvFlexCollisionGeometry> shapeGeometry;
	NvFlexVector<Vec4> shapePositions;
	NvFlexVector<Quat> shapeRotations;
	NvFlexVector<Vec4> shapePrevPositions;
	NvFlexVector<Quat> shapePrevRotations;
	NvFlexVector<int> shapeFlags;

	// rigids
	NvFlexVector<int> rigidOffsets;
	NvFlexVector<int> rigidIndices;
	NvFlexVector<int> rigidMeshSize;
	NvFlexVector<float> rigidCoefficients;
	NvFlexVector<float> rigidPlasticThresholds;
	NvFlexVector<float> rigidPlasticCreeps;
	NvFlexVector<Quat> rigidRotations;
	NvFlexVector<Vec3> rigidTranslations;
	NvFlexVector<Vec3> rigidLocalPositions;
	NvFlexVector<Vec4> rigidLocalNormals;

	// inflatables
	NvFlexVector<int> inflatableTriOffsets;
	NvFlexVector<int> inflatableTriCounts;
	NvFlexVector<float> inflatableVolumes;
	NvFlexVector<float> inflatableCoefficients;
	NvFlexVector<float> inflatablePressures;

	// springs
	NvFlexVector<int> springIndices;
	NvFlexVector<float> springLengths;
	NvFlexVector<float> springStiffness;

	NvFlexVector<int> triangles;
	NvFlexVector<Vec3> triangleNormals;
	NvFlexVector<Vec3> uvs;

	SimBuffers(NvFlexLibrary* l) :
		positions(l), restPositions(l), velocities(l), phases(l), densities(l),
		anisotropy1(l), anisotropy2(l), anisotropy3(l), normals(l), smoothPositions(l),
		diffusePositions(l), diffuseVelocities(l), diffuseCount(l), activeIndices(l),
		shapeGeometry(l), shapePositions(l), shapeRotations(l), shapePrevPositions(l),
		shapePrevRotations(l),	shapeFlags(l), rigidOffsets(l), rigidIndices(l), rigidMeshSize(l),
		rigidCoefficients(l), rigidPlasticThresholds(l), rigidPlasticCreeps(l), rigidRotations(l), rigidTranslations(l),
		rigidLocalPositions(l), rigidLocalNormals(l), inflatableTriOffsets(l),
		inflatableTriCounts(l), inflatableVolumes(l), inflatableCoefficients(l),
		inflatablePressures(l), springIndices(l), springLengths(l),
		springStiffness(l), triangles(l), triangleNormals(l), uvs(l)
	{}
};


struct Instance
{
	Instance(const char* mesh) :

		mFile(mesh),
		mColor(0.5f, 0.5f, 1.0f),
		mRadius(0.1f),
		mScale(2.0f),
		mTranslation(0.0f, 1.0f, 0.0f),

		mClusterSpacing(1.0f),
		mClusterRadius(0.0f),
		mClusterStiffness(0.5f),

		mLinkRadius(0.0f),
		mLinkStiffness(1.0f),

		mGlobalStiffness(0.0f),

		mSurfaceSampling(0.0f),
		mVolumeSampling(4.0f),

		mSkinningFalloff(2.0f),
		mSkinningMaxDistance(100.0f),

		mClusterPlasticThreshold(0.0f),
		mClusterPlasticCreep(0.0f)
	{}

	const char* mFile;
	Vec3 mColor;

	Vec3 mScale;
	Vec3 mTranslation;
	float mRadius;
	float mClusterSpacing;
	float mClusterRadius;
	float mClusterStiffness;

	float mLinkRadius;
	float mLinkStiffness;

	float mGlobalStiffness;

	float mSurfaceSampling;
	float mVolumeSampling;

	float mSkinningFalloff;
	float mSkinningMaxDistance;

	float mClusterPlasticThreshold;
	float mClusterPlasticCreep;
};

struct RenderingInstance
{
	Mesh* mMesh;
	std::vector<int> mSkinningIndices;
	std::vector<float> mSkinningWeights;
	std::vector<Vec3> mRigidRestPoses;
	Vec3 mColor;
	int mOffset;
};

class FlexClass
{
public:

	FlexClass();
	~FlexClass();

	void initialize();
	void preupdate();
	void update();
	void shutdown();
	
	void AddSphere(float radius, Vec3 position, Quat rotation);



	SimBuffers * AllocBuffers(NvFlexLibrary * lib);

	void DestroyBuffers(SimBuffers * buffers);

	void CreateSoftBody(Instance instance, int group);

	void CalculateRigidLocalPositions(const Vec4 * restPositions, const int * offsets, const Vec3 * translations, const int * indices, int numRigids, Vec3 * localPositions);

	void ClearShapes();

	void AddCapsule(float radius, float halfHeight, Vec3 position, Quat rotation, Vec3 prevposition, Quat prevrotation);

	void AddBox(Vec3 halfEdge = Vec3(2.0f), Vec3 center = Vec3(0.0f), Quat quat = Quat(), Vec3 prevposition = Vec3(2.0f), Quat prevrotation = Quat(), bool dynamic = false, int channels = eNvFlexPhaseShapeChannelMask);

	std::vector<Vec4> particle;

private:
	NvFlexLibrary* library;
	NvFlexSolver* solver;

	NvFlexParams g_params;
	SimBuffers* g_buffers;

	int g_numExtraParticles = 0;
	int g_numExtraMultiplier = 1;
	bool g_shapesChanged = false;
	bool plasticDeformation;
	float mRadius;

	std::vector<Instance> mInstances;
	std::vector<RenderingInstance> mRenderingInstances;

	void MapBuffers(SimBuffers* buffers);
	void UnmapBuffers(SimBuffers* buffers);
};