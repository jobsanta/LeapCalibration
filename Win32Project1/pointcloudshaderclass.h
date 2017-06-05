////////////////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _POINTCLOUDSHADERCLASS_H_
#define _POINTCLOUDSHADERCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include "Camera.h"

using namespace DirectX;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: PointCloudShaderClass
////////////////////////////////////////////////////////////////////////////////
class PointCloudShaderClass
{
private:

	struct CBChangesEveryFrame
	{
		XMMATRIX View;
		XMMATRIX Projection;
		XMFLOAT4 XYScale;
		XMFLOAT3 camera;
	};

public:
	PointCloudShaderClass();
	PointCloudShaderClass(const PointCloudShaderClass&);
	~PointCloudShaderClass();

	bool Initialize(ID3D11Device*, HWND, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT3);
	bool UpdateSubResource(ID3D11DeviceContext*, FLOAT*, FLOAT*);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*, int, int);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT3);
	void RenderShader(ID3D11DeviceContext*);

private:
	HRESULT LoadShaders(ID3D11Device*);
	ID3D11VertexShader*                 m_pVertexShader;
	ID3D11PixelShader*                  m_pPixelShader;
	ID3D11GeometryShader*               m_pGeometryShader;

	ID3D11InputLayout* m_layout;
	ID3D11SamplerState*                 m_pColorSampler;

	ID3D11Buffer*                       m_pVertexBuffer;
	ID3D11Buffer*                       m_pCBChangesEveryFrame;

	ID3D11Texture2D*                    m_pDepthTexture2D;
	ID3D11ShaderResourceView*           m_pDepthTextureRV;

	ID3D11Texture2D*                    m_pColorTexture2D;
	ID3D11ShaderResourceView*           m_pColorTextureRV;
	ID3D11RasterizerState* pState = NULL;
	XMMATRIX                            m_projection;
	float                               m_xyScale;

	ID3D11InputLayout*                  m_pVertexLayout;

	int depthWidth;
	int depthHeight;

	Camera mCam;
};

#endif