
struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 PosW  : POSITION;
	float3 NormW : NORMAL;
	float2 TexC  : TEXCOORD;
	float3 TanW  : TANGENT;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 WorldMatrix;
	float4x4 WorldInverseTransposeMatrix;
	float4x4 WorldViewProjectionMatrix;
	float4x4 TexTransform;
	Material MaterialObj;
};