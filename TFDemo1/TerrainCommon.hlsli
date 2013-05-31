

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION;
	float3 NormW   : NORMAL;
	float4 TexC    : TEXCOORD0;
	float3 TanW    : TANGENT;
	float4 ProjTex : TEXCOORD1;
	float  ClipD   : SV_ClipDistance;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 WorldMatrix;
	float4x4 WorldInverseTransposeMatrix;
	float4x4 WorldViewProjectionMatrix;
	float4x4 TexTransform;
	float4x4 LightWVPT;
}

cbuffer cbPerFrame : register(b1)
{
	float4 ClipData;
}