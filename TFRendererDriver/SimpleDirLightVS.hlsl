#include "SimpleDirLightCommon.h"

VertexOut VS( VertexIn vin )
{
	VertexOut vout;
	
	// Execute transformations
	vout.PosH  = mul(float4(vin.PosL, 1.0f), WorldViewProjectionMatrix);
	vout.PosW  = mul(float4(vin.PosL, 1.0f), WorldMatrix);
	vout.NormW = mul(float4(vin.Normal, 0.0f), WorldInverseTransposeMatrix);
	vout.TexC  = vin.TexC;
    
    return vout;
}