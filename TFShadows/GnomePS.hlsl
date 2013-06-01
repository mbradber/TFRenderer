
#include "GnomeCommon.hlsli"

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


float4 main( VertexOut pin ) : SV_TARGET
{
	// Renormalize after transformation
	pin.NormW = normalize(pin.NormW);


	// Sample the diffuse map for the crate
	float4 _texColor = DiffuseMap.Sample(samAnisotropic, pin.TexC);

	// calculate color based on light direction against normal 
	float4 _lightVec     = float4(LightObj.Direction, 0.0f) * -1.0f;
	float  _lambert      = max(0, dot(_lightVec, pin.NormW));
	float4 _diffuseLight = _lambert * LightObj.Diffuse;
	float4 _ambientLight = LightObj.Ambient;

	return (_texColor * _diffuseLight) + (_texColor * _ambientLight);
}