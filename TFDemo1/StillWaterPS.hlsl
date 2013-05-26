

#include "StillWaterCommon.hlsli"

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

// Textures
Texture2D ReflectionMap : register(t0);

// Samplers
SamplerState samAnisotropic         : register(s0);
SamplerState samLinear              : register(s1);
SamplerComparisonState samShadowPCF : register(s2);
SamplerState pointSampler           : register(s3);


float4 main(VertexOut pin) : SV_TARGET
{
	// Renormalize after transformation
	pin.NormW = normalize(pin.NormW);

	//return ReflectionMap.Sample(samAnisotropic, pin.TexC.zw);
	//return ReflectionMap.Sample(samAnisotropic, pin.PosH.xy);
	float4 ProjCoords = pin.PosND;
	ProjCoords /= ProjCoords.w;
	ProjCoords.x = ProjCoords.x / 2.0f + 0.5f;
	ProjCoords.y = ProjCoords.y / -2.0f + 0.5f;

	return ReflectionMap.Sample(samAnisotropic, ProjCoords.xy);
}