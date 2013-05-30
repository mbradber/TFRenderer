

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
Texture2D NormalMap     : register(t1);

// Samplers
SamplerState samAnisotropic         : register(s0);
SamplerState samLinear              : register(s1);
SamplerComparisonState samShadowPCF : register(s2);
SamplerState pointSampler           : register(s3);

float3 DistortNormal(float3 a_f3BumpedNormal, float2 a_f2TexCoords)
{
	a_f3BumpedNormal.x += a_f2TexCoords.x;
	a_f3BumpedNormal.z += a_f2TexCoords.y;

	return normalize(a_f3BumpedNormal);
}

float3 GetBumpedNormal(float2 a_f2TexCoords, VertexOut pin)
{
	float4 _normalSample = NormalMap.Sample(samLinear, a_f2TexCoords);
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

	return _bumpedNormal;
}


float4 main(VertexOut pin) : SV_TARGET
{
	float dx = 5.0f / 512.0f;

	// Renormalize after transformation
	pin.NormW = normalize(pin.NormW);

	// sample reflection map
	float4 ProjCoords = pin.PosND;
	ProjCoords /= ProjCoords.w;
	ProjCoords.x = ProjCoords.x / 2.0f + 0.5f;
	ProjCoords.y = ProjCoords.y / -2.0f + 0.5f;


	//float4 _normalSample = NormalMap.Sample(samLinear, pin.TexC.xy);
	//float3 _normalT      = (2.0f * _normalSample - float4(1.0f, 1.0f, 1.0f, 1.0f)).xyz;
	//// Get the (tangent - projection of tangent onto normal) vector
	//float3 T = normalize(pin.TanW - (dot(pin.TanW, pin.NormW) * pin.NormW));
	//float3 N = pin.NormW;
	//float3 B = cross(N, T);

	//// construct transform from tangent to object space...
	//float3x3 TBN = float3x3(T, B, N);
	//// calculate "bumped normal" as sample normal in object space
	//float3 _bumpedNormal = mul(_normalT, TBN);
	//// bring bumped normal from object space to world space
	//_bumpedNormal = normalize(mul(float4(_bumpedNormal, 0.0f), WorldMatrix).xyz);

	float3 _bumpedNormal = GetBumpedNormal(pin.TexC.zw, pin);
	float3 _bumpedNormalNeg = GetBumpedNormal(pin.TexC.xy, pin);
	_bumpedNormal += _bumpedNormalNeg;
	_bumpedNormal = normalize(_bumpedNormal);

	// sample with distortion
	float2 _f2SampleCoords = ProjCoords.xy;
	_f2SampleCoords.x += _bumpedNormal.x * dx;
	_f2SampleCoords.y += _bumpedNormal.z * dx;
	float4 _f4Reflection = ReflectionMap.Sample(samAnisotropic, _f2SampleCoords);

	//_bumpedNormal = DistortNormal(_bumpedNormal, pin.ProjTex.xy);


	// calculate color based on light direction against normal 
	float4 _lightVec     = float4(LightObj.Direction, 0.0f) * -1.0f;
	float  _lambert      = max(0, dot(_lightVec, _bumpedNormal));

	float4 _f4Diffuse = _lambert * LightObj.Diffuse * MaterialObj.Diffuse;
	float4 _f4AmbientLight = LightObj.Ambient * MaterialObj.Ambient;
	float4 _f4FinalColor = (_f4Diffuse * _f4Reflection) + (_f4AmbientLight * _f4Reflection);
	_f4FinalColor.a = 0.70f;

	return _f4FinalColor;
}