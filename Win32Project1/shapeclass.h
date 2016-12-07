#include "GeometryGenerator.h"
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
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

	int GetCylinderIndexCount();
	int GetBoxIndexCount();
	int GetCylinderIndexOffset();
	int GetBoxIndexOffset();
	int GetBoxVertexOffset();
	int GetCylinderVertexOffset();

private:
	void ShutdownBuffers();
	void RenderBuffers(ID3D11Device*);
	bool BuildGeometryBuffers(ID3D11Device* device);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	int mBoxVertexOffset;
	int mCylinderVertexOffset;

	UINT mBoxIndexOffset;
	UINT mCylinderIndexOffset;

	UINT mBoxIndexCount;
	UINT mCylinderIndexCount;

};