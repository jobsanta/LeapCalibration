////////////////////////////////////////////////////////////////////////////////
// Filename: ground.fx
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////


Texture2D shadowTexture : register(t0);
SamplerState SampleTypeClamp : register(s0);


cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;

};

cbuffer LightBuffer2
{
	float3 lightPosition;
	float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float4 position : POSITION;
	float3 normal : NORMAL;
};


struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float4 viewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the position of the vertice as viewed by the camera in a separate variable.
	output.viewPosition = output.position;

	// Store the texture coordinates for the pixel shader.
	output.color = float4(1.0,1.0,1.0,1.0);

	// Calculate the normal vector against the world matrix only.
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// Normalize the normal vector.
	output.normal = normalize(output.normal);

	// Calculate the position of the vertex in the world.
	worldPosition = mul(input.position, worldMatrix);

	// Determine the light position based on the position of the light and the position of the vertex in the world.
	output.lightPos = lightPosition.xyz - worldPosition.xyz;

	// Normalize the light position vector.
	output.lightPos = normalize(output.lightPos);

	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////

float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
	float4 color;
float lightIntensity;
float2 projectTexCoord;
float shadowValue;

// Set the default output color to the ambient light value for all pixels.
color = ambientColor;

// Calculate the amount of light on this pixel.
lightIntensity = saturate(dot(input.normal, input.lightPos));
if (lightIntensity > 0.0f)
{
	// Determine the light color based on the diffuse color and the amount of light intensity.
	color += (diffuseColor * lightIntensity);

	// Saturate the light color.
	color = saturate(color);
}


// Calculate the projected texture coordinates to be used with the shadow texture.
projectTexCoord.x = input.viewPosition.x / input.viewPosition.w / 2.0f + 0.5f;
projectTexCoord.y = -input.viewPosition.y / input.viewPosition.w / 2.0f + 0.5f;

// Sample the shadow value from the shadow texture using the sampler at the projected texture coordinate location.
shadowValue = shadowTexture.Sample(SampleTypeClamp, projectTexCoord).r;

// Combine the shadows with the final color.
color = color * shadowValue;

return color;
}