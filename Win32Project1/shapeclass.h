#include "GeometryGenerator.h"
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
};

class ShapeClass
{
public:
	ShapeClass();
	~ShapeClass();
	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11Device*);

	int GetBoxIndexOffset();
	int GetBoxVertexOffset();
	int GetBoxIndexCount();
	int GetCylinderIndexCount();
	int GetCylinderIndexOffset();
	int GetCylinderVertexOffset();
	int GetSphereIndexCount();
	int GetSphereIndexOffset();
	int GetSphereVertexOffset();
	int GetGridIndexCount();
	int GetGridVertexOffset();
	int GetGridIndexOffset();

private:
	void ShutdownBuffers();
	void RenderBuffers(ID3D11Device*);
	bool BuildGeometryBuffers(ID3D11Device* device);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	int mBoxVertexOffset;
	int mCylinderVertexOffset;
	int mSphereVertexOffset;
	int mGridVertexOffset;

	UINT mBoxIndexOffset;
	UINT mCylinderIndexOffset;
	UINT mSphereIndexOffset;
	UINT mGridIndexOffset;

	UINT mBoxIndexCount;
	UINT mCylinderIndexCount;
	UINT mSphereIndexCount;
	UINT mGridIndexCount;
};