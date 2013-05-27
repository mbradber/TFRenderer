
#include "TerrainCommon.hlsli"

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

	// ClipData.x holds the reflection surface world position Y value (height).
	// ClipData.y holds a flag value component to determine whether or not enable this clip distance.
	// For render passes that do not use this the value will be 0, for render passes (Such as rendering into
	// the reflection map) that do use it, the value will be 1.
	//
	// We do not want to render anything "below" this surface in the reflection so we will clip
	// all reversed geometry that is ABOVE the reflective surface in world space (which means its actually
	// below it).
	vout.ClipD = (ClipData.x - vout.PosW.y) * ClipData.y;

	return vout;
}