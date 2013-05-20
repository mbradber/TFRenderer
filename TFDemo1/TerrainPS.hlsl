

#include "TerrainCommon.hlsli"

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
Texture2D GrassLayer : register(t0);
Texture2D DirtLayer  : register(t1);
Texture2D BlendLayer : register(t2);

// Samplers
SamplerState samAnisotropic         : register(s0);
SamplerState samLinear              : register(s1);
SamplerComparisonState samShadowPCF : register(s2);
SamplerState pointSampler           : register(s3);

float4 main(VertexOut pin) : SV_TARGET
{
	// Renormalize after transformation
	pin.NormW = normalize(pin.NormW);

	// Sample textures
	float4 _f4GrassColor = GrassLayer.Sample(samAnisotropic, pin.TexC.xy);
	float4 _f4DirtColor  = DirtLayer.Sample(samAnisotropic, pin.TexC.xy);
	float4 _f4BlendValue = BlendLayer.Sample(samAnisotropic, pin.TexC.zw);

	// Calculate lighting factors
	float4 _f4LightVec = float4(LightObj.Direction, 0.0f) * -1.0f;
	float  _fLambert   = max(0, dot(_f4LightVec, pin.NormW));
	float4 _f4DiffuseLight = _fLambert * LightObj.Diffuse;
	float4 _f4AmbientLight = LightObj.Ambient;

	// Calculate final color
	_f4GrassColor *= _f4BlendValue.r;
	_f4DirtColor  *= _f4BlendValue.g;

	// return alpha blended terrain color
	return _f4GrassColor + _f4DirtColor;
}