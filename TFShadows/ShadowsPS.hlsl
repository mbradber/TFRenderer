
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

Texture2D    DiffuseMap     : register(t0);
Texture2D    NormalMap      : register(t1);
Texture2D    ShadowMapFront : register(t2);
Texture2D    ShadowMapBack  : register(t3);
SamplerState samAnisotropic : register(s0);
SamplerState samLinear      : register(s1);
//SamplerComparisonState samShadow      : register(s2);
SamplerState samShadow : register(s2);

float CalcShadow(float4 a_vShadowPosH)
{
	a_vShadowPosH.xyz /= a_vShadowPosH.w;
	float _fDepth = a_vShadowPosH.z;

	float dx = 1.0f / 2048.0f;

	float _fShadowFactor = 0.0f;

	//float2 _pcfGrid[9] = 
	//{
	//	float2(-dx, -dx),  float2(0.0f, -dx), float2(dx, -dx),
	//	float2(-dx, 0.0f), float2(0.0f, 0),   float2(dx, 0.0f),
	//	float2(-dx, dx),   float2(0.0f, dx),  float2(dx, dx),
	//};

	//[unroll]
	//for(int i = 0; i < 9; ++i)
	//{
	//	_fShadowFactor += ShadowMapFront.SampleCmpLevelZero(samShadow, a_vShadowPosH.xy + _pcfGrid[i], _fDepth);
	//}


	//float _fShadowFactor = ShadowMapFront.SampleCmpLevelZero(samShadow, a_vShadowPosH.xy, _fDepth);
	float _fShadowDepthFront = ShadowMapFront.Sample(samShadow, a_vShadowPosH.xy);
	float _fShadowDepthBack  = ShadowMapBack.Sample(samShadow, a_vShadowPosH.xy);
	float _fAvgDepth = (_fShadowDepthFront + _fShadowDepthBack) / 2.0f;

	//return _fDepth <= _fShadowDepthBack;

	float2 _offsets[4] = 
	{
		float2(-dx, -dx), float2(-dx, dx), float2(dx, -dx), float2(dx, dx)
	};

	[unroll]
	for(int i = 0; i < 4; ++i)
	{
		float _fSample = ShadowMapFront.Sample(samShadow, a_vShadowPosH.xy + _offsets[i]);
		_fShadowFactor += _fSample;
	}

	[branch]
	if(_fDepth <= _fAvgDepth)
	{



		return _fShadowFactor / 4.0f;

		//return 1.0f;
	}
	else
	{
		return 0.0f;
	}

	//return _fShadowFactor / 9.0f;
}

float4 main( VertexOut pin ) : SV_TARGET
{
	// get shadow factor
	float _fShadowFactor = 1.0f;

	// sample shadow map using projective texture coords
	//float4 _sampledDepthVector = ShadowMapFront.Sample(samShadow, pin.ProjTex.xy);
	//float _sampledDepth = _sampledDepthVector.r;
	//bool _bInShadow = _fDepth > _sampledDepth;

	// Renormalize after transformation
	pin.NormW = normalize(pin.NormW);

	// Get normal sample and decompress it
	float4 _normalSample = NormalMap.Sample(samLinear, pin.TexC); 
	float3 _normalT      = (2.0f * _normalSample - float4(1.0f, 1.0f, 1.0f, 1.0f)).xyz;
	// Get the (tangent - projection of tangent onto normal) vector
	float3 T = normalize(pin.TanW - (dot(pin.TanW, pin.NormW) * pin.NormW));
	float3 N = pin.NormW;
	float3 B = cross(N, T);

	// construct transform from tanget to object space...
	float3x3 TBN = float3x3(T, B, N);
	// calculate "bumped normal" as sample normal in object space
	float3 _bumpedNormal = mul(_normalT, TBN);
	// bring bumped normal from object space to world space
	_bumpedNormal = mul(float4(_bumpedNormal, 0.0f), WorldMatrix).xyz;

	// Sample the diffuse map for the crate
	//float4 _texColor = DiffuseMap.Sample(samAnisotropic, pin.TexC);
	//float4 _texColor = ShadowMapFront.Sample(samLinear, pin.ProjTex.xy);
	float4 _texColor = ShadowMapFront.Sample(samLinear, pin.TexC);

	// calculate color based on light direction against normal 
	float4 _lightVec     = float4(LightObj.Direction, 0.0f) * -1.0f;
	float  _lambert      = max(0, dot(_lightVec, pin.NormW));
	//float _lambert = max(0, dot(_lightVec, float4(_bumpedNormal, 0.0f)));
	float4 _diffuseLight = _lambert * LightObj.Diffuse * MaterialObj.Diffuse;
	float4 _ambientLight = LightObj.Ambient * MaterialObj.Ambient;
	float4 _color        = _texColor * (_ambientLight + _diffuseLight);

	// don't bother calculating shadow factor if this pixel isn't receiving 'enough' light...
	if(_lambert > 0.25)
	{
		_fShadowFactor = CalcShadow(pin.ProjTex);
	}
	
	return _color *= _fShadowFactor;


	//return _color;
}