#include "SkyCommon.hlsli"

TextureCube CubeMap : register(t0);
SamplerState samTriLinear : register(s0); // this is actually anisotropic

float4 main(VertexOut pin) : SV_Target
{
	float4 _color = CubeMap.Sample(samTriLinear, pin.PosL);
	return _color;
}