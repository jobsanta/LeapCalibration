////////////////////////////////////////////////////////////////////////////////
// Filename: shadow.vs
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////

Texture2D depthMapTexture : register(t0);
Texture2D depthMapTexture2 : register(t1);
SamplerState SampleTypeClamp : register(s0);

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
	matrix lightViewMatrix2;
	matrix lightProjectionMatrix2;
};

//////////////////////
// CONSTANT BUFFERS //
//////////////////////
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
	float3 normal : NORMAL;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
	float4 lightViewPosition2 : TEXCOORD3;
	float3 lightPos2 : TEXCOORD4;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ShadowVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;
	float3 lightPosition2;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPosition = mul(input.position, worldMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	// Calculate the position of the vertice as viewed by the second light source.
	output.lightViewPosition2 = mul(input.position, worldMatrix);
	output.lightViewPosition2 = mul(output.lightViewPosition2, lightViewMatrix2);
	output.lightViewPosition2 = mul(output.lightViewPosition2, lightProjectionMatrix2);

	// Store the texture coordinates for the pixel shader.

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
	lightPosition2 = lightPosition;
	lightPosition2.z = -lightPosition.z;
	// Determine the second light position based on the position of the light and the position of the vertex in the world.
	output.lightPos2 = lightPosition2.xyz - worldPosition.xyz;

	// Normalize the second light position vector.
	output.lightPos2 = normalize(output.lightPos2);

	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ShadowPixelShader(PixelInputType input) : SV_TARGET
{
	float bias;
float4 color;
float2 projectTexCoord;
float depthValue;
float lightDepthValue;
float lightIntensity;

// Set the bias value for fixing the floating point precision issues.
bias = 0.001f;

// Set the default output color to be black (shadow).
color = float4(0.0f, 0.0f, 0.0f, 1.0f);

// Calculate the projected texture coordinates.
projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
{
	// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
	depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;

	// Calculate the depth of the light.
	lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

	// Subtract the bias from the lightDepthValue.
	lightDepthValue = lightDepthValue - bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
	if (lightDepthValue < depthValue)
	{
		// Calculate the amount of light on this pixel.
		lightIntensity = saturate(dot(input.normal, input.lightPos));

		// If this pixel is illuminated then set it to pure white (non-shadow).
		if (lightIntensity > 0.0f)
		{
			color += float4(1.0f, 1.0f, 1.0f, 1.0f)*lightIntensity;
		}
	}
}

// Second light.
projectTexCoord.x = input.lightViewPosition2.x / input.lightViewPosition2.w / 2.0f + 0.5f;
projectTexCoord.y = -input.lightViewPosition2.y / input.lightViewPosition2.w / 2.0f + 0.5f;

if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
{
	depthValue = depthMapTexture2.Sample(SampleTypeClamp, projectTexCoord).r;

	lightDepthValue = input.lightViewPosition2.z / input.lightViewPosition2.w;
	lightDepthValue = lightDepthValue - bias;

	if (lightDepthValue < depthValue)
	{
		lightIntensity = saturate(dot(input.normal, input.lightPos2));
		if (lightIntensity > 0.0f)
		{
			color += float4(1.0f, 1.0f, 1.0f, 1.0f)*lightIntensity;
		}
	}
}
color = saturate(color);
return color;
}