
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
	vout.PosND   = mul(float4(vin.PosL, 1.0f), WorldViewProjectionMatrix);
	vout.NormW   = mul(float4(vin.NormL, 0.0f), WorldInverseTransposeMatrix);
	vout.ProjTex = vin.TexC; // save these coords before transform
	vout.TexC    = mul(float4(vin.TexC.xy, 0, 1), TexTransform);
	vout.TanW    = mul(float4(vin.TanU, 0.0f), WorldMatrix);

	return vout;
}