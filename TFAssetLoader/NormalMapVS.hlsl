#include "NormalMap.hlsli"

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormL   : NORMAL;
	float3 Tangent : TANGENT;
	float2 TexC    : TEXCOORD;
};

VertexOut main( VertexIn vin ) : SV_POSITION
{
	return pos;
}