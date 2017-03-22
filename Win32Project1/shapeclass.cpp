#include "shapeclass.h"

ShapeClass::ShapeClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

ShapeClass::~ShapeClass()
{
}

void ShapeClass::Shutdown()
{
	ShutdownBuffers();
}

bool ShapeClass::Initialize(ID3D11Device* device)
{
	bool result;
	result = BuildGeometryBuffers(device);
	return result;

}


bool ShapeClass::BuildGeometryBuffers(ID3D11Device* device)
{
	HRESULT result;

	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateCylinder(0.5f,0.5f, 1.0f, 20, 20, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset = 0;
	mCylinderVertexOffset = box.Vertices.size();

	// Cache the index count of each object.
	mBoxIndexCount = box.Indices.size();
	mCylinderIndexCount = cylinder.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset = 0;
	mCylinderIndexOffset = mBoxIndexCount;

	UINT totalVertexCount =
		box.Vertices.size() +
		cylinder.Vertices.size();

	UINT totalIndexCount =
		mBoxIndexCount +
		mCylinderIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex> vertices(totalVertexCount);

	XMFLOAT4 Green(0.0f, 1.0f, 0.0f, 1.0f);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Color = Green;
		vertices[k].Normal = box.Vertices[i].Normal;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Color = Green;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(Vertex) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	result = device->CreateBuffer(&vbd, &vinitData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	
	//
	// Pack the indices of all the meshes into one index buffer.
	//
		std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	// Create the index buffer.
	result = device->CreateBuffer(&ibd, &iinitData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	indices.clear();
	vertices.clear();
	m_vertexCount = totalVertexCount;
	m_indexCount = totalIndexCount;

}

void ShapeClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ShapeClass::RenderBuffers(ID3D11Device* device)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;
	ID3D11DeviceContext* context;
	device->GetImmediateContext(&context);
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void ShapeClass::Render(ID3D11Device* device)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(device);

}

int ShapeClass::GetBoxIndexCount()
{
	return mBoxIndexCount;
}

int ShapeClass::GetCylinderIndexCount()
{
	return mCylinderIndexCount;
}

int ShapeClass::GetBoxIndexOffset()
{
	return mBoxIndexOffset;
}

int ShapeClass::GetCylinderIndexOffset()
{
	return mCylinderIndexOffset;
}

int ShapeClass::GetBoxVertexOffset()
{
	return mBoxVertexOffset;
}

int ShapeClass::GetCylinderVertexOffset()
{
	return mCylinderVertexOffset;
}