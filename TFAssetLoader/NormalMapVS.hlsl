#include "NormalMap.hlsli"

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormL   : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TanL    : TANGENT;
};

VertexOut main( VertexIn vin )
{
	VertexOut vout;

	vout.PosH   = mul(float4(vin.PosL, 1.0f), WorldViewProjectionMatrix);
	vout.PosW   = mul(float4(vin.PosL, 1.0f), WorldMatrix).xyz;
	vout.NormW  = mul(float4(vin.NormL, 0.0f), WorldInverseTransposeMatrix).xyz;
	vout.TexC   = vin.TexC;
	vout.TanW   = mul(float4(vin.TanL, 0.0f), WorldMatrix).xyz;

	return vout;
}