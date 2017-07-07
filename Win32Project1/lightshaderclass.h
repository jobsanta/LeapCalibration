////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTSHADERCLASS_H_
#define _LIGHTSHADERCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "LightHelper.h"
using namespace std;
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: LightShaderClass
////////////////////////////////////////////////////////////////////////////////
class LightShaderClass
{
public:
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

	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void Render(ID3D11DeviceContext*, int, int, int);
	bool SetShaderPerFrame(ID3D11DeviceContext*, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4, XMFLOAT4, float range, XMFLOAT3 attenuate);
	bool SetShaderPerObject(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, Material, ID3D11ShaderResourceView*, XMFLOAT3);
private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3DBlob*, HWND, WCHAR*);
//	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4, XMFLOAT3, float);
	void RenderShader(ID3D11DeviceContext*, int, int, int);

private:
	//ID3D11Effect* m_effect;
	//ID3D11EffectTechnique* m_technique;
	//ID3D11InputLayout* m_layout;

	//ID3D11EffectMatrixVariable* m_worldMatrixPtr;
	//ID3D11EffectMatrixVariable* m_viewMatrixPtr;
	//ID3D11EffectMatrixVariable* m_projectionMatrixPtr;
	//ID3D11EffectShaderResourceVariable* m_texturePtr;
	//ID3D11EffectVectorVariable* m_lightDirectionPtr;
	//ID3D11EffectVectorVariable* m_ambientColorPtr;
	//ID3D11EffectVectorVariable* m_diffuseColorPtr;
	//ID3D11EffectScalarVariable* m_CameraDistancePtr;

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11Buffer* m_matrixPerframe;
	ID3D11Buffer* m_matrixPerObject;
};
#endif