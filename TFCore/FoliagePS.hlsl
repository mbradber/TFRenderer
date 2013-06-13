
#include "FoliageCommon.hlsli"

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

Texture2D    DiffuseMap             : register(t0);
Texture2D    NormalMap              : register(t1);
Texture2D    ShadowMapFront         : register(t2);
Texture2D    ShadowMapBack          : register(t3);
SamplerState samAnisotropic         : register(s0);
SamplerState samLinear              : register(s1);
SamplerComparisonState samShadowPCF : register(s2);
SamplerState pointSampler           : register(s3);

float CalcShadow(float4 a_vShadowPosH, float3 a_vNorm, float3 a_vLightVec)
{
	a_vShadowPosH.xyz /= a_vShadowPosH.w;
	float _fDepth = a_vShadowPosH.z;

	float dx = 1.0f / 2048.0f;
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

float4 main( VertexOut pin ) : SV_TARGET
{
	// Renormalize after transformation
	pin.NormW = normalize(pin.NormW);

	// Sample the diffuse map for the crate
	float4 _texColor = DiffuseMap.Sample(samAnisotropic, pin.TexC);
	float _texColorSum = _texColor.r + _texColor.g + _texColor.b;
	clip(_texColorSum - 0.05);

	// calculate color based on light direction against normal 
	float4 _lightVec     = float4(LightObj.Direction, 0.0f) * -1.0f;
	float  _lambert      = max(0, dot(_lightVec.xyz, pin.NormW));
	float4 _diffuseLight = _lambert * LightObj.Diffuse;
	float4 _ambientLight = LightObj.Ambient;

	float _fShadowFactor = 1.0f;
	
	return (_texColor * _diffuseLight) * _fShadowFactor + (_texColor * _ambientLight);
}