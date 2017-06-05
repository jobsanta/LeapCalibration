////////////////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _COLORSHADERCLASS_H_
#define _COLORSHADERCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include "LightHelper.h"
using namespace DirectX;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: ColorShaderClass
////////////////////////////////////////////////////////////////////////////////
class ColorShaderClass
{
private:

	struct cbPerObject
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		Material gColor;
		XMFLOAT3 camera;
		float z_offset;
	};

	struct cbPerFrame
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT4 specularColor;
		XMFLOAT3 lightPosition;
		float range;
		XMFLOAT3 Attenuate;
		float padding;
	};

public:
	ColorShaderClass();
	ColorShaderClass(const ColorShaderClass&);
	~ColorShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int, int);
	bool SetShaderPerFrame(ID3D11DeviceContext*, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4, XMFLOAT4, float range, XMFLOAT3 attenuate);
	bool SetShaderPerObject(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, Material, XMFLOAT3);
private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	void RenderShader(ID3D11DeviceContext*, int, int, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11Buffer* m_matrixPerframe;
	ID3D11Buffer* m_matrixPerObject;
};

#endif