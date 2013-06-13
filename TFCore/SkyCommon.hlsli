
struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float3 PosL    : POSITION;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 WorldViewProjectionMatrix;
}