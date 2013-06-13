#include "SkyCommon.hlsli"

struct VertexIn
{
	float3 PosL  : POSITION;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;

	// Set z = w so that z/w = 1 (skydome will always be on the far plane)
	vout.PosH = mul(float4(vin.PosL, 1.0f), WorldViewProjectionMatrix).xyww;
	vout.PosL = vin.PosL;
	return vout;
}

