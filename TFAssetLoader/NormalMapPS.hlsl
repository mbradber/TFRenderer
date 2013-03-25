#include "NormalMap.hlsli"

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float  _pad;
};

cbuffer cbPerFrame : register(b1)
{
	DirectionalLight LightObj;
	float3 EyePosition;
};

Texture2D    DiffuseMap     : register(t0);
Texture2D    NormalMap      : register(t1);
SamplerState samAnisotropic : register(s0);

float4 main(VertexOut pin) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}