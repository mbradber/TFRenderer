





struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float  _pad;
};

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 WorldMatrix;
	float4x4 WorldInverseTransposeMatrix;
	float4x4 WorldViewProjectionMatrix;
	Material MaterialObj;
}

cbuffer cbPerFrame : register(b1)
{
	DirectionalLight LightObj;
	float3 EyePosition;
}

Texture2D DiffuseMap : register(t0);

SamplerState samAnisotropic
{
	Filter=ANISOTROPIC;
	MaxAnisotropy=4;

	AddressU = WRAP;
	AddressV = WRAP;
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