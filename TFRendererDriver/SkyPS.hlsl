#include "SkyCommon.hlsli"

TextureCube CubeMap : register(t0);
SamplerState samTriLinear : register(s0);

float4 PS(VertexOut pin) : SV_Target
{
	float4 _color = CubeMap.Sample(samTriLinear, pin.PosL);
	//float4 _color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	return _color;
}