

struct VertexOut
{
	float3 PosH  : SV_POSITION;
	float3 PosW  : POSITION;
	float3 NormW : NORMAL;
	float2 TexC  : TEXCOORD;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 WorldMatrix;
	float4x4 WorldInverseTransposeMatrix;
	float4x4 WorldViewProjectionMatrix;
	float4x4 TexTransform;
	Material MaterialObj;
};