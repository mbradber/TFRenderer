
struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float3 PosL    : POSITION;
};

// TODO: 	-We use the same cbPerObject in all shaders, this isn't necessary and updating this large
//           resource every frame is wasteful
cbuffer cbPerObject : register(b0)
{
	float4x4 WorldMatrix;
	float4x4 WorldInverseTransposeMatrix;
	float4x4 WorldViewProjectionMatrix;
	float4x4 TexTransform;
	float4x4 LightWVPT;
	Material MaterialObj;
}