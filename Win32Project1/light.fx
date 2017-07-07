////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
Texture2D shadowTexture  : register(t0);
Texture2D shaderTexture : register(t1);


SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap  : register(s1);

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; // w = SpecPower
	float4 Reflect;
};

cbuffer cbPerFrame : register(b0)
{
	float4 ambientColor;
	float4 diffuseColor;
	float4 specularColor;
	float3 lightPosition;
	float range;
	float3 att;
	float padding;
};

cbuffer cbPerObject : register(b1)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	Material gMat;
	float3 camera;
	float zoffset;
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
	float3 posW: POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 viewPosition : TEXCOORD1;
};

void ComputePointLight(Material mat, float3 LPosition, float lightRange, float4 lightAmbient, float4 lightDiffuse, float4 lightSpec,
	float3 lightAtt, float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = LPosition - pos;

	// The distance from surface to light.
	float d = length(lightVec);

	// Range test.
	if (d > lightRange)
		return;

	// Normalize the light vector.
	lightVec /= d;
	// Ambient term.
	ambient = mat.Ambient * lightAmbient;

	// Add diffuse and specular term, provided the surface is in
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * lightDiffuse;
		spec = specFactor * mat.Specular * lightSpec;
	}

	//// Attenuate
	float att = 1.0f / dot(lightAtt, float3(1.0f, d, d*d));

	diffuse *= att;
	spec *= att;
}

void ComputeDirectionalLight(Material mat, float3 Direction, float4 lightAmbient, float4 lightDiffuse, float4 lightSpec,
	float3 normal, float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = Direction;

	// Add ambient term.
	ambient = mat.Ambient * lightAmbient;

	// Add diffuse and specular term, provided the surface is in
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * lightDiffuse;
		spec = specFactor * mat.Specular * lightSpec;
	}
}


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;
	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.posW = output.position.xyz;

	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the position of the vertice as viewed by the camera in a separate variable.
	output.viewPosition = output.position;

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only.
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	if (output.position.w != 0.0f)
	{
		output.position = output.position*((length(output.posW - camera) + zoffset) / output.position.w);
	}

	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_Target
{
input.normal = normalize(input.normal);
float3 toEyeW = normalize(camera - input.posW);
float2 projectTexCoord;
float shadowValue;
toEyeW = normalize(toEyeW);
float4 textureColor;


float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

float3 lightDirection = float3(0, -1, 0);

float4 A, D, S;
ComputePointLight(gMat, lightPosition, range, ambientColor, diffuseColor, specularColor,
	att, input.posW, input.normal, toEyeW, A, D, S);
ambient += A;
diffuse += D;
spec += S;
//ComputeDirectionalLight(gMat, lightDirection, ambientColor, diffuseColor, specularColor,
//	 input.normal, toEyeW, A, D, S);
//ambient += A;
//diffuse += D;
//spec += S;

float3 lightMirror = float3(lightPosition.x, lightPosition.y, -lightPosition.z);

ComputePointLight(gMat, lightMirror, range, ambientColor, diffuseColor, specularColor,
	att, input.posW, input.normal, toEyeW, A, D, S);
ambient += A;
diffuse += D;
spec += S;

// Sample the pixel color from the texture using the sampler at this texture coordinate location.
textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);


// Calculate the projected texture coordinates to be used with the shadow texture.
projectTexCoord.x = input.viewPosition.x / input.viewPosition.w / 2.0f + 0.5f;
projectTexCoord.y = -input.viewPosition.y / input.viewPosition.w / 2.0f + 0.5f;

// Sample the shadow value from the shadow texture using the sampler at the projected texture coordinate location.
shadowValue = shadowTexture.Sample(SampleTypeClamp, projectTexCoord).r;

// Combine the shadows with the final color.
float4 litColor = textureColor*(ambient + diffuse + spec)*shadowValue;
litColor.a = gMat.Diffuse.a;
return litColor;

}