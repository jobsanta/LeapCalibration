////////////////////////////////////////////////////////////////////////////////
// Filename: PointCloudShaderClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pointcloudshaderclass.h"

PointCloudShaderClass::PointCloudShaderClass()
{
	m_pVertexShader = 0;
	m_pPixelShader = 0;
	m_pGeometryShader = 0;
	m_layout = 0;
	m_pColorSampler = 0;
	depthWidth = 0;
	depthHeight = 0;
}

PointCloudShaderClass::PointCloudShaderClass(const PointCloudShaderClass& other)
{
}

PointCloudShaderClass::~PointCloudShaderClass()
{
}

bool PointCloudShaderClass::Initialize(ID3D11Device* device, HWND hwnd, int depthWidth, int depthHeight)
{
	bool result;

	mCam.SetLens(0.25f*XM_PI, 1280.0 / 800.0, 1.0f, 1000.0f);

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../Win32Project1/pointcloud.fx", L"../Win32Project1/pointcloud.fx", depthWidth, depthHeight);
	if (!result)
	{
		return false;
	}

	return true;
}

void PointCloudShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool PointCloudShaderClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, XMFLOAT3 camera)
{
	bool result;

	SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, camera);

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext);

	return true;
}

bool PointCloudShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, int m_depthWidth, int m_depthHeight)
{
	depthWidth = m_depthWidth;
	depthHeight = m_depthHeight;

	// Create depth texture
	D3D11_TEXTURE2D_DESC depthTexDesc = { 0 };
	depthTexDesc.Width = m_depthWidth;
	depthTexDesc.Height = m_depthHeight;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	depthTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	depthTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	depthTexDesc.MiscFlags = 0;

	device->CreateTexture2D(&depthTexDesc, NULL, &m_pDepthTexture2D);
	device->CreateShaderResourceView(m_pDepthTexture2D, NULL, &m_pDepthTextureRV);

	// Create color texture
	D3D11_TEXTURE2D_DESC colorTexDesc = { 0 };
	colorTexDesc.Width = m_depthWidth;
	colorTexDesc.Height = m_depthHeight;
	colorTexDesc.MipLevels = 1;
	colorTexDesc.ArraySize = 1;
	colorTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	colorTexDesc.SampleDesc.Count = 1;
	colorTexDesc.SampleDesc.Quality = 0;
	colorTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	colorTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	colorTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	colorTexDesc.MiscFlags = 0;

	device->CreateTexture2D(&colorTexDesc, NULL, &m_pColorTexture2D);

	device->CreateShaderResourceView(m_pColorTexture2D, NULL, &m_pColorTextureRV);

	LoadShaders(device);

	// Create the vertex buffer
	D3D11_BUFFER_DESC bd = { 0 };
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(short);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	device->CreateBuffer(&bd, NULL, &m_pVertexBuffer);
	ID3D11DeviceContext* context;
	device->GetImmediateContext(&context);
	context->IASetInputLayout(m_pVertexLayout);

	// Set vertex buffer
	UINT stride = 0;
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Create the constant buffers
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBChangesEveryFrame);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, NULL, &m_pCBChangesEveryFrame);

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, &m_pColorSampler);

	// Initialize the projection matrix
	m_projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1280 / static_cast<FLOAT>(800), 0.1f, 100.f);

	// Calculate correct XY scaling factor so that our vertices are correctly placed in the world
	// This helps us to unproject from the Kinect's depth camera back to a 3d world
	// Since the Horizontal and Vertical FOVs are proportional with the sensor's resolution along those axes
	// We only need to do this for horizontal
	// I.e. tan(horizontalFOV)/depthWidth == tan(verticalFOV)/depthHeight
	// Essentially we're computing the vector that light comes in on for a given pixel on the depth camera
	// We can then scale our x&y depth position by this and the depth to get how far along that vector we are
	const float DegreesToRadians = 3.14159265359f / 180.0f;
	m_xyScale = tanf(60 * DegreesToRadians * 0.5f) / (m_depthWidth * 0.5f);

	// Set rasterizer state to disable backface culling
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.DepthBias = false;
	rasterDesc.DepthBiasClamp = 0;
	rasterDesc.SlopeScaledDepthBias = 0;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.AntialiasedLineEnable = false;

	device->CreateRasterizerState(&rasterDesc, &pState);

	context->RSSetState(pState);

	//pState->Release();

	return true;
}

void PointCloudShaderClass::ShutdownShader()
{
	// Release the matrix constant buffer.
	if (m_pCBChangesEveryFrame)
	{
		m_pCBChangesEveryFrame->Release();
		m_pCBChangesEveryFrame = 0;
	}

	// Release the light constant buffer.
	if (m_pGeometryShader)
	{
		m_pGeometryShader->Release();
		m_pGeometryShader = 0;
	}
	if (m_pColorTexture2D)
	{
		m_pColorTexture2D->Release();
		m_pColorTexture2D = 0;
	}
	if (m_pColorTextureRV)
	{
		m_pColorTextureRV->Release();
		m_pColorTextureRV = 0;
	}

	if (m_pDepthTexture2D)
	{
		m_pDepthTexture2D->Release();
		m_pDepthTexture2D = 0;
	}
	if (m_pDepthTextureRV)
	{
		m_pDepthTextureRV->Release();
		m_pDepthTextureRV = 0;
	}

	if (m_pColorSampler)
	{
		m_pColorSampler->Release();
		m_pColorSampler = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pPixelShader)
	{
		m_pPixelShader->Release();
		m_pPixelShader = 0;
	}

	// Release the vertex shader.
	if (m_pVertexShader)
	{
		m_pVertexShader->Release();
		m_pVertexShader = 0;
	}

	return;
}

void PointCloudShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool PointCloudShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, XMFLOAT3 camera)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	mCam.UpdateViewMatrix();
	XMMATRIX view = mCam.View();
	XMMATRIX proj = mCam.Proj();
	XMMATRIX viewProj = mCam.ViewProj();

	CBChangesEveryFrame cb;
	cb.View = XMMatrixTranspose(viewMatrix);
	cb.Projection = XMMatrixTranspose(projectionMatrix);
	cb.XYScale = XMFLOAT4(m_xyScale, -m_xyScale, 0.f, 0.f);
	cb.camera = camera;
	deviceContext->UpdateSubresource(m_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);

	return true;
}

bool PointCloudShaderClass::UpdateSubResource(ID3D11DeviceContext* device, FLOAT* depthDest, FLOAT* rgbDest)
{
	D3D11_MAPPED_SUBRESOURCE msT;
	HRESULT result;

	//float depth[512];
	//for (int i = 0; i < 512; i++)
	//	depth[i] = depthDest[i];

	result = device->Map(m_pDepthTexture2D, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msT);
	if (FAILED(result)) { return false; }
	memcpy(msT.pData, depthDest, sizeof(FLOAT)*depthWidth*depthHeight * 3);
	device->Unmap(m_pDepthTexture2D, NULL);

	result = device->Map(m_pColorTexture2D, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msT);
	if (FAILED(result)) { return false; }

	memcpy(msT.pData, rgbDest, sizeof(float)*depthWidth*depthHeight * 4);
	device->Unmap(m_pColorTexture2D, NULL);

	return true;
}

void PointCloudShaderClass::RenderShader(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetInputLayout(m_pVertexLayout);

	// Set vertex buffer
	UINT stride = 0;
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	deviceContext->RSSetState(pState);
	// Set up shaders
	deviceContext->VSSetShader(m_pVertexShader, NULL, 0);

	deviceContext->GSSetShader(m_pGeometryShader, NULL, 0);
	deviceContext->GSSetConstantBuffers(0, 1, &m_pCBChangesEveryFrame);
	deviceContext->GSSetShaderResources(0, 1, &m_pDepthTextureRV);
	deviceContext->GSSetShaderResources(1, 1, &m_pColorTextureRV);
	deviceContext->GSSetSamplers(0, 1, &m_pColorSampler);

	deviceContext->PSSetShader(m_pPixelShader, NULL, 0);

	// Draw the scene
	deviceContext->Draw(depthWidth * depthHeight, 0);

	deviceContext->GSSetShader(0, NULL, 0);

	return;
}

HRESULT PointCloudShaderClass::LoadShaders(ID3D11Device* device)
{
	// Compile the geometry shader
	ID3D10Blob* pBlob = NULL;
	ID3D10Blob* errorMessage = NULL;
	HRESULT hr = D3DCompileFromFile(L"../Win32Project1/pointcloud.fx", NULL, NULL, "GS", "gs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pBlob, &errorMessage);
	if (FAILED(hr)) { return hr; };

	// Create the geometry shader
	hr = device->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &m_pGeometryShader);
	pBlob->Release();
	if (FAILED(hr)) { return hr; }

	// Compile the pixel shader
	hr = D3DCompileFromFile(L"../Win32Project1/pointcloud.fx", NULL, NULL, "PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pBlob, &errorMessage);
	if (FAILED(hr)) { return hr; }

	// Create the pixel shader
	hr = device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &m_pPixelShader);
	pBlob->Release();
	if (FAILED(hr)) { return hr; }

	// Compile the vertex shader
	hr = D3DCompileFromFile(L"../Win32Project1/pointcloud.fx", NULL, NULL, "VS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pBlob, &errorMessage);
	if (FAILED(hr)) { return hr; }

	// Create the vertex shader
	hr = device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &m_pVertexShader);
	if (SUCCEEDED(hr))
	{
		// Define the vertex input layout
		D3D11_INPUT_ELEMENT_DESC layout[] = { { "POSITION", 0, DXGI_FORMAT_R16_SINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } };

		// Create the vertex input layout
		hr = device->CreateInputLayout(layout, ARRAYSIZE(layout), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_pVertexLayout);
	}

	pBlob->Release();
	if (FAILED(hr)) { return hr; }

	// Set the input vertex layout
	// In this case we don't actually use it for anything
	// All the work is done in the geometry shader, but we need something here
	// We only need to set this once since we have only one vertex format
	ID3D11DeviceContext* context;
	device->GetImmediateContext(&context);
	context->IASetInputLayout(m_pVertexLayout);

	return hr;
}