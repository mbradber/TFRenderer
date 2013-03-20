//#include "SkyCommon.h"

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

TextureCube CubeMap;
SamplerState samTriLinear : register(s0);

float4 PS(VertexOut pin) : SV_Target
{
	float4 _color = CubeMap.Sample(samTriLinear, pin.PosL);
	return _color;
}