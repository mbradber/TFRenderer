
#include "ShadowsCommon.hlsli"

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

	//float _fDot = dot(a_vNorm, a_vLightVec);
	//if(_fDot < 0.2)
	//{
	//	return 0;
	//}

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

	// Get normal sample and decompress it
	float4 _normalSample = NormalMap.Sample(samLinear, pin.TexC); 
	float3 _normalT      = (2.0f * _normalSample - float4(1.0f, 1.0f, 1.0f, 1.0f)).xyz;
	// Get the (tangent - projection of tangent onto normal) vector
	float3 T = normalize(pin.TanW - (dot(pin.TanW, pin.NormW) * pin.NormW));
	float3 N = pin.NormW;
	float3 B = cross(N, T);

	// construct transform from tangent to object space...
	float3x3 TBN = float3x3(T, B, N);
	// calculate "bumped normal" as sample normal in object space
	float3 _bumpedNormal = mul(_normalT, TBN);
	// bring bumped normal from object space to world space
	_bumpedNormal = normalize(mul(float4(_bumpedNormal, 0.0f), WorldMatrix).xyz);

	// Sample the diffuse map for the crate
	//float4 _texColor = DiffuseMap.Sample(samAnisotropic, pin.TexC);

	//float4 _texColor = ShadowMapFront.Sample(samLinear, pin.ProjTex.xy);
	float4 _texColor = ShadowMapFront.Sample(samLinear, pin.TexC);

	// calculate color based on light direction against normal 
	float4 _lightVec     = float4(LightObj.Direction, 0.0f) * -1.0f;
	float  _lambert      = max(0, dot(_lightVec, pin.NormW));
	//float _lambert = max(0, dot(_lightVec, float4(_bumpedNormal, 0.0f)));
	float4 _diffuseLight = _lambert * LightObj.Diffuse;
	float4 _ambientLight = LightObj.Ambient;

	float _fShadowFactor = 1.0f;
	//_fShadowFactor = CalcShadow(pin.ProjTex, pin.NormW, _lightVec.xyz);
	
	return (_texColor * _diffuseLight) * _fShadowFactor + (_texColor * _ambientLight);
}