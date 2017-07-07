////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightshaderclass.h"

LightShaderClass::LightShaderClass()
{
	//m_effect = 0;
	//m_technique = 0;
	//m_layout = 0;

	//m_worldMatrixPtr = 0;
	//m_viewMatrixPtr = 0;
	//m_projectionMatrixPtr = 0;
	//m_texturePtr = 0;
	//m_lightDirectionPtr = 0;
	//m_diffuseColorPtr = 0;
	//m_ambientColorPtr = 0;

	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleStateWrap = 0;
	m_sampleStateClamp = 0;
	m_matrixPerframe = 0;
	m_matrixPerObject = 0;
}

LightShaderClass::LightShaderClass(const LightShaderClass& other)
{
}

LightShaderClass::~LightShaderClass()
{
}

bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Initialize the shader that will be used to draw the triangle.
	result = InitializeShader(device, hwnd, L"../Win32Project1/light.fx", L"../Win32Project1/light.fx");
	if (!result)
	{
		return false;
	}

	return true;
}

void LightShaderClass::Shutdown()
{
	// Shutdown the shader effect.
	ShutdownShader();

	return;
}

void LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, int indexOffset, int vertexOffset)
{

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount, indexOffset, vertexOffset);

}
bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsfilename, WCHAR* psfilename)
{
	HRESULT result;
	ID3DBlob* errorMessage;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc2;

	// Initialize the error message.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Load the shader in from the file.
	result = D3DCompileFromFile(vsfilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsfilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsfilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}
	result = D3DCompileFromFile(psfilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psfilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, psfilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//Create the vertex shader from the buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "NORMAL";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "TEXCOORD";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateWrap);
	if (FAILED(result))
	{
		return false;
	}
	// Create a clamp texture sampler state description.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateClamp);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(cbPerObject);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixPerObject);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(cbPerFrame);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_matrixPerframe);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void LightShaderClass::ShutdownShader()
{ // Release the matrix constant buffer.
	if (m_matrixPerframe)
	{
		m_matrixPerframe->Release();
		m_matrixPerframe = 0;
	}

	// Release the light constant buffer.
	if (m_matrixPerObject)
	{
		m_matrixPerObject->Release();
		m_matrixPerObject = 0;
	}

	// Release the sampler state.
	if (m_sampleStateWrap)
	{
		m_sampleStateWrap->Release();
		m_sampleStateWrap = 0;
	}
	if (m_sampleStateClamp)
	{
		m_sampleStateClamp->Release();
		m_sampleStateClamp = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void LightShaderClass::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
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

//bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
//	ID3D11ShaderResourceView* texture,
//	ID3D11ShaderResourceView* shadowTexture, XMFLOAT3 lightPosition, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 cameraDistance, float z_offset)
//{
//	HRESULT result;
//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//	unsigned int bufferNumber;
//	MatrixBufferType* dataPtr;
//	LightBufferType* dataPtr2;
//	LightBufferType2* dataPtr3;
//
//	XMMATRIX world, view, proj, lightView, lightProj;
//
//	// Transpose the matrices to prepare them for the shader.
//	world = XMMatrixTranspose(worldMatrix);
//	view = XMMatrixTranspose(viewMatrix);
//	proj = XMMatrixTranspose(projectionMatrix);
//
//	// Lock the constant buffer so it can be written to.
//	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// Get a pointer to the data in the constant buffer.
//	dataPtr = (MatrixBufferType*)mappedResource.pData;
//
//	// Copy the matrices into the constant buffer.
//	dataPtr->world = world;
//	dataPtr->view = view;
//	dataPtr->projection = proj;
//	dataPtr->camera = cameraDistance;
//	dataPtr->z_offset = z_offset;
//
//	// Unlock the constant buffer.
//	deviceContext->Unmap(m_matrixBuffer, 0);
//
//	// Set the position of the constant buffer in the vertex shader.
//	bufferNumber = 0;
//
//	// Now set the constant buffer in the vertex shader with the updated values.
//	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
//
//	// Set shader texture resource in the pixel shader.
//	deviceContext->PSSetShaderResources(0, 1, &texture);
//
//	deviceContext->PSSetShaderResources(1, 1, &shadowTexture);
//
//	// Lock the light constant buffer so it can be written to.
//	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// Get a pointer to the data in the constant buffer.
//	dataPtr2 = (LightBufferType*)mappedResource.pData;
//	//The ambient light color is mapped into the light buffer and then set as a constant in the pixel shader before rendering.
//
//	// Copy the lighting variables into the constant buffer.
//	dataPtr2->ambientColor = ambientColor;
//	dataPtr2->diffuseColor = diffuseColor;
//
//	// Unlock the constant buffer.
//	deviceContext->Unmap(m_lightBuffer, 0);
//
//	// Set the position of the light constant buffer in the pixel shader.
//	bufferNumber = 0;
//
//	// Finally set the light constant buffer in the pixel shader with the updated values.
//	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);
//
//	// Lock the second light constant buffer so it can be written to.
//	result = deviceContext->Map(m_lightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// Get a pointer to the data in the constant buffer.
//	dataPtr3 = (LightBufferType2*)mappedResource.pData;
//
//	// Copy the lighting variables into the constant buffer.
//	dataPtr3->lightPosition = lightPosition;
//	dataPtr3->padding = 0.0f;
//
//	// Unlock the constant buffer.
//	deviceContext->Unmap(m_lightBuffer2, 0);
//
//	// Set the position of the light constant buffer in the vertex shader.
//	bufferNumber = 1;
//
//	// Finally set the light constant buffer in the pixel shader with the updated values.
//	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer2);
//
//	return true;
//}

bool LightShaderClass::SetShaderPerFrame(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* depthMapTexture, XMFLOAT3 lightPosition,
	XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, float range, XMFLOAT3 attenuate)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	cbPerFrame* dataPtr;
	unsigned int bufferNumber;

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixPerframe, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (cbPerFrame*)mappedResource.pData;
	//The ambient light color is mapped into the light buffer and then set as a constant in the pixel shader before rendering.

	// Copy the lighting variables into the constant buffer.
	dataPtr->ambientColor = ambientColor;
	dataPtr->diffuseColor = diffuseColor;
	dataPtr->specularColor = ambientColor;
	dataPtr->lightPosition = lightPosition;
	dataPtr->range = range;
	dataPtr->Attenuate = attenuate;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixPerframe, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_matrixPerframe);

	deviceContext->PSSetShaderResources(0, 1, &depthMapTexture);

	return true;
}


bool LightShaderClass::SetShaderPerObject(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, Material color, ID3D11ShaderResourceView* texture, XMFLOAT3 camera)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	cbPerObject* dataPtr;
	unsigned int bufferNumber;

	XMMATRIX world, view, proj, lightView, lightProj;

	// Transpose the matrices to prepare them for the shader.
	world = XMMatrixTranspose(worldMatrix);
	view = XMMatrixTranspose(viewMatrix);
	proj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (cbPerObject*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = proj;
	dataPtr->gColor = color;
	dataPtr->camera = camera;
	dataPtr->z_offset = 0;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixPerObject, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 1;
	deviceContext->PSSetShaderResources(1, 1, &texture);
	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixPerObject);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_matrixPerObject);

	return true;
}
void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, int indexOffset, int vertexOffset)
{
	// Set the input layout.
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Get the description structure of the technique from inside the shader so it can be used for rendering.
	deviceContext->PSSetSamplers(0, 1, &m_sampleStateClamp);
	deviceContext->PSSetSamplers(1, 1, &m_sampleStateWrap);

	// Go through each pass in the technique (should be just one currently) and render the triangles.
	deviceContext->DrawIndexed(indexCount, indexOffset, vertexOffset);

	return;
}