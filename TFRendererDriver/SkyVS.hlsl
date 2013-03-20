//#include "SkyCommon.h"

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 WorldViewProjectionMatrix;
}

struct VertexIn
{
	float3 PosL : POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Set z = w so that z/w = 1 (skydome will always be on the far plane)
	vout.PosH = mul(float4(vin.PosL, 1.0f), WorldViewProjectionMatrix).xyww;

	vout.PosL = vin.PosL;

	return vout;
}

