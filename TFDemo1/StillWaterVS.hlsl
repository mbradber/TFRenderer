
#include "StillWaterCommon.hlsli"

struct VertexIn
{
	float3 PosL  : POSITION;
	float3 NormL : NORMAL;
	float4 TexC  : TEXCOORD;
	float3 TanU  : TANGENT;
};

VertexOut main( VertexIn vin )
{
	VertexOut vout;

	// per vertex transforms
	vout.PosH    = mul(float4(vin.PosL, 1.0f), WorldViewProjectionMatrix);
	vout.PosW    = mul(float4(vin.PosL, 1.0f), WorldMatrix);
	vout.NormW   = mul(float4(vin.NormL, 0.0f), WorldInverseTransposeMatrix);
	vout.TexC    = vin.TexC;
	vout.TanW    = mul(float4(vin.TanU, 0.0f), WorldMatrix);
	vout.ProjTex = mul(float4(vin.PosL, 1.0f), LightWVPT);

	return vout;
}