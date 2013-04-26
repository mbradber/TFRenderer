

struct VertexIn
{
	float3 PosL  : POSITION;
	float3 NormL : NORMAL;
	float2 TexC  : TEXCOORD;
	float3 TanL  : TANGENT;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
};

// TODO: Consider binding this buffer (and possibly other gpu resources)
// to other input slots so we don't have to swap resources at b0
// when we render with a second shader
cbuffer cbPerObject : register(b0)
{
	float4x4 WorldViewProjection;
};

