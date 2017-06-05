////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////

Texture2D shaderTexture : register(t0);
Texture2D shadowTexture  : register(t1);

SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap  : register(s1);

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 camera;
	float zoffset;
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
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 viewPosition : TEXCOORD1;
	float3 lightPos:TEXCOORD2;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;
	float3 screen;
	float4 worldPosition;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	worldPosition = mul(input.position, worldMatrix);

	screen = float3(output.position.x, output.position.y, output.position.z);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.viewPosition = output.position;

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only.
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// Normalize the normal vector.
	output.normal = normalize(output.normal);

	// Determine the light position based on the position of the light and the position of the vertex in the world.
	output.lightPos = lightPosition.xyz - worldPosition.xyz;

	// Normalize the light position vector.
	output.lightPos = normalize(output.lightPos);

	if (output.position.w != 0.0f)
	{
		output.position = output.position*((length(screen - camera) + zoffset) / output.position.w);
	}

	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_Target
{
	float4 color;
float2 projectTexCoord;
float4 textureColor;
float lightIntensity;
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

// Sample the pixel color from the texture using the sampler at this texture coordinate location.
textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);

// Combine the light and texture color.
color = color * textureColor;

// Calculate the projected texture coordinates to be used with the shadow texture.
projectTexCoord.x = input.viewPosition.x / input.viewPosition.w / 2.0f + 0.5f;
projectTexCoord.y = -input.viewPosition.y / input.viewPosition.w / 2.0f + 0.5f;

// Sample the shadow value from the shadow texture using the sampler at the projected texture coordinate location.
shadowValue = shadowTexture.Sample(SampleTypeClamp, projectTexCoord).r;

// Combine the shadows with the final color.
color = color * shadowValue;

return color;
}