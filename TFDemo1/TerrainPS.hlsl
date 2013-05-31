

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
Texture2D GrassLayer      : register(t0);
Texture2D DirtLayer       : register(t1);
Texture2D DullGrassLayer  : register(t2);
Texture2D DarkDirtLayer   : register(t3);
Texture2D BlendLayerGrass : register(t4);
Texture2D BlendLayerDirt  : register(t5);   
Texture2D ShadowMapFront  : register(t6);

// Samplers
SamplerState samAnisotropic         : register(s0);
SamplerState samLinear              : register(s1);
SamplerComparisonState samShadowPCF : register(s2);
SamplerState pointSampler           : register(s3);

float CalcShadow(float4 a_vShadowPosH)
{
	float dx = 1.0f / 2048.0f;

	a_vShadowPosH.xyz /= a_vShadowPosH.w;
	float _fDepth = a_vShadowPosH.z;
	float _fShadowFactor = 0.0f;

	float2 _pcfGrid[9] = 
	{
		float2(-dx, -dx),  float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0),   float2(dx, 0.0f),
		float2(-dx, dx),   float2(0.0f, dx),  float2(dx, dx),
	};
	 
	[unroll]
	for(int i = 0; i < 9; ++i)
	{
		_fShadowFactor += ShadowMapFront.SampleCmpLevelZero(samShadowPCF, a_vShadowPosH.xy + _pcfGrid[i], _fDepth);
	}

	return _fShadowFactor / 9.0f;
}

float4 main(VertexOut pin) : SV_TARGET
{
	// Renormalize after transformation
	pin.NormW = normalize(pin.NormW);

	// Sample textures
	float4 _f4GrassColor      = GrassLayer.Sample(samAnisotropic, pin.TexC.xy);
	float4 _f4DirtColor       = DirtLayer.Sample(samAnisotropic, pin.TexC.xy);
	float4 _f4DarkDirtColor   = DarkDirtLayer.Sample(samAnisotropic, pin.TexC.xy);
	float4 _f4DullGrassColor  = DullGrassLayer.Sample(samAnisotropic, pin.TexC.xy);
	float4 _f4BlendValueGrass = BlendLayerGrass.Sample(samAnisotropic, pin.TexC.zw);
	float4 _f4BlendValueDirt  = BlendLayerDirt.Sample(samAnisotropic, pin.TexC.zw);

	// Calculate lighting factors
	float4 _f4LightVec = float4(LightObj.Direction, 0.0f) * -1.0f;
	float  _fLambert   = max(0, dot(_f4LightVec, pin.NormW));
	float4 _f4DiffuseLight = _fLambert * LightObj.Diffuse;
	float4 _f4AmbientLight = LightObj.Ambient;

	// Calculate blended colors
	float4 _f4GrassColorBlended = _f4GrassColor * (_f4BlendValueGrass.r * _f4BlendValueDirt.b);
	_f4GrassColor = (_f4GrassColorBlended * _f4DiffuseLight)
		+ (_f4GrassColorBlended * _f4AmbientLight);

	float4 _f4DullGrassColorBlended = _f4DullGrassColor * (_f4BlendValueGrass.g * _f4BlendValueDirt.b);
	_f4DullGrassColor = (_f4DullGrassColorBlended * _f4DiffuseLight)
		+ (_f4DullGrassColorBlended * _f4AmbientLight);

	float4 _f4DirtColorBlended = _f4DirtColor * _f4BlendValueDirt.r;
	_f4DirtColor = (_f4DirtColorBlended * _f4DiffuseLight)
		+ (_f4DirtColorBlended * _f4AmbientLight);

	float4 _f4DarkDirtColorBlended = _f4DarkDirtColor * _f4BlendValueDirt.g;
	_f4DarkDirtColor = (_f4DarkDirtColorBlended * _f4DiffuseLight)
		+ (_f4DarkDirtColorBlended * _f4AmbientLight);

	float _fShadowFactor = CalcShadow(pin.ProjTex);

	// return alpha blended terrain color
	float4 _f4TerrainColor = _f4GrassColor + _f4DullGrassColor + _f4DirtColor + _f4DarkDirtColor;
	float4 _f4Diffuse = _f4TerrainColor * _fShadowFactor;
	float4 _f4Ambient = _f4TerrainColor * 0.25;

	return _f4Ambient + _f4Diffuse;
}