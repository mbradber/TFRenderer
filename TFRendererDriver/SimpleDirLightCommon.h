
struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
};

struct VertexIn
{
	float3 PosL   : POSITION;
    float3 Normal : NORMAL;
	float2 TexC   : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 PosW  : POSITION;
	float4 NormW : NORMAL;
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