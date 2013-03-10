#include "SimpleDirLightCommon.h"

Texture2D DiffuseMap : register(t0);
SamplerState samAnisotropic : register(s0);

float4 PS( VertexOut pin ) : SV_Target
{
	// Renormalize after transformation
	pin.NormW = normalize(pin.NormW);
	// Sample the diffuse map for the crate
	float4 _texColor = DiffuseMap.Sample(samAnisotropic, pin.TexC);

	float4 _lightVec     = float4(LightObj.Direction, 0.0f) * -1.0f;
	float  _lambert      = max(0, dot(_lightVec, pin.NormW));
	float4 _diffuseLight = _lambert * LightObj.Diffuse;
	float4 _color        = _texColor * (LightObj.Ambient + _diffuseLight);

	return _color;
}