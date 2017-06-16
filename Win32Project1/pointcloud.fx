//------------------------------------------------------------------------------
// <copyright file="DepthWithColor-D3D.fx" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

Texture2D<float3>    txDepth  : register(t0);
Texture2D<float4> txColor  : register(t1);
Texture2D<float3> txDist : register(t2);
SamplerState      samColor : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer cbChangesEveryFrame : register(b0)
{
	matrix  View;
	matrix  Projection;
	float4  XYScale;
	float3 camera;
};

//--------------------------------------------------------------------------------------
// Constants
//--------------------------------------------------------------------------------------
static const int DepthWidth = 512;
static const int DepthHeight = 424;
static const float2 DepthWidthHeight = float2(DepthWidth, DepthHeight);
static const float2 DepthHalfWidthHeight = DepthWidthHeight / 2.0;
static const float2 DepthHalfWidthHeightOffset = DepthHalfWidthHeight - 0.5;
static const float2 ColorWidthHeight = float2(512, 424);

// vertex offsets for building a quad from a depth pixel
static const float4 quadOffsets[4] =
{
	float4(-0.5, -0.5, 0, 0),
	float4(0.5, -0.5, 0, 0),
	float4(-0.5,  0.5, 0, 0),
	float4(0.5,  0.5, 0, 0)
};

// texture lookup offsets for sampling current and nearby depth pixels
static const float2 texOffsets4Samples[4] =
{
	float2(0, 0),
	float2(1, 0),
	float2(0, 1),
	float2(1, 1)
};

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct GS_INPUT
{
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 Col : COLOR;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
GS_INPUT VS()
{
	GS_INPUT output = (GS_INPUT)0;

	return output;
}

//--------------------------------------------------------------------------------------
// Geometry Shader
//
// Takes in a single vertex point.  Expands it into the 4 vertices of a quad.
// Depth is sampled from a texture passed in of the Kinect's depth output.
// Color is sampled from a texture passed in of the Kinect's color output mapped to depth space.
//--------------------------------------------------------------------------------------
[maxvertexcount(4)]
void GS(point GS_INPUT particles[1], uint primID : SV_PrimitiveID, inout TriangleStream<PS_INPUT> triStream)
{
	PS_INPUT output;

	// texture load location for the pixel we're on
	int3 baseLookupCoords = int3(primID % DepthWidth, primID / DepthWidth, 0);

	float3 pos = txDepth.Load(baseLookupCoords);
	float3 dist = txDist.Load(baseLookupCoords);

	// set the base world position here so we don't have to do it per vertex
	// convert x and y lookup coords to world space meters
	float4 WorldPos;
	WorldPos.xyz = pos+dist;
	WorldPos.w = 1.0;

	// convert to camera space
	float4 ViewPos = mul(WorldPos, View);

	// base color texture sample lookup coords, in [0,1]
	float2 colorTextureCoords = baseLookupCoords.xy / ColorWidthHeight;

	//// determine how large to make the point sprite - scale it up a little to fill in holes
	//// also make it larger if it is further away to prevent aliasing artifacts
	static const float4 PointSpriteScaleFactor = float4(1.0 / DepthWidth, 1.0 / DepthHeight, 0.0, 0.0) * 4;
	float4 quadOffsetScalingFactorInViewspace = PointSpriteScaleFactor * pos.z;

	[unroll]
	for (uint c = 0; c < 4; ++c)
	{
		// expand the quad in camera space so that it's always the same size irrespective of camera angle
		float4 ViewPosExpanded = ViewPos + quadOffsets[c] * quadOffsetScalingFactorInViewspace;

		// then project it
		output.Pos = mul(ViewPosExpanded, Projection);

		if (output.Pos.w != 0.0f)
		{
			output.Pos = output.Pos*((length(WorldPos.xyz - camera)) / output.Pos.w);
		}

		// sample the color texture for the corner we're at
		output.Col = txColor.SampleLevel(samColor, colorTextureCoords + texOffsets4Samples[c], 0);

		triStream.Append(output);
	}
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	return float4(input.Col.rgb, 1.0);
}