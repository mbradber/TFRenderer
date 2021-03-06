

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float4 PosND   : POSITION0;
	float3 NormW   : NORMAL;
	float4 TexC    : TEXCOORD0;
	float3 TanW    : TANGENT;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 WorldMatrix;
	float4x4 WorldInverseTransposeMatrix;
	float4x4 WorldViewProjectionMatrix;
	float4x4 TexTransform;
	float4x4 TexTransformNeg;
}