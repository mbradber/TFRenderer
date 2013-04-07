#include "RenderDepthCommon.hlsli"


float4 main(VertexOut pin) : SV_TARGET
{
	return pin.PosH.z;
}