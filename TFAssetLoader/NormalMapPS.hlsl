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
	// Renormalize after transformation
	pin.NormW = normalize(pin.NormW);

	// Get normal sample and decompress it
	float4 _normalSample = NormalMap.Sample(samAnisotropic, pin.TexC);
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
	float4 _texColor = DiffuseMap.Sample(samAnisotropic, pin.TexC);

	// calculate color based on light direction against normal 
	float4 _lightVec     = float4(LightObj.Direction, 0.0f) * -1.0f;
	//float  _lambert      = max(0, dot(_lightVec, pin.NormW));
	float _lambert = max(0, dot(_lightVec, float4(_bumpedNormal, 0.0f)));
	float4 _diffuseLight = _lambert * LightObj.Diffuse * MaterialObj.Diffuse;
	float4 _ambientLight = LightObj.Ambient * MaterialObj.Ambient;
	float4 _color        = _texColor * (_ambientLight + _diffuseLight);

	return _color;
}