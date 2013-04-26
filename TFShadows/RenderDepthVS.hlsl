#include "RenderDepthCommon.hlsli"

VertexOut main(VertexIn vin)
{
	VertexOut vout;

	vout.PosH   = mul(float4(vin.PosL, 1.0f), WorldViewProjection);

	return vout;
}