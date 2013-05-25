

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
//Texture2D GrassLayer      : register(t0);
//Texture2D DirtLayer       : register(t1);
//Texture2D DullGrassLayer  : register(t2);
//Texture2D DarkDirtLayer   : register(t3);
//Texture2D BlendLayerGrass : register(t4);
//Texture2D BlendLayerDirt  : register(t5);   

// Samplers
SamplerState samAnisotropic         : register(s0);
SamplerState samLinear              : register(s1);
SamplerComparisonState samShadowPCF : register(s2);
SamplerState pointSampler           : register(s3);


float4 main(VertexOut pin) : SV_TARGET
{
	// Renormalize after transformation
	pin.NormW = normalize(pin.NormW);

	return float4(0.0f, 0.0f, 1.0f, 1.0f);
}