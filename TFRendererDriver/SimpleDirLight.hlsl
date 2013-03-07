

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
//cbuffer ConstantBuffer : register( b0 )
//{
//	matrix World;
//	matrix View;
//	matrix Projection; 
//}

//struct DirectionalLight
//{
//	float4 Ambient;
//	float4 Diffuse;
//	float4 Specular;
//	float3 Direction;
//	float  _pad;
//};
//
//struct Material
//{
//	float4 Ambient;
//	float4 Diffuse;
//	float4 Specular;
//};



cbuffer cbPerObject : register(cb0)
{
	//float4x4 WorldMatrix;
	//float4x4 WorldInverseTransposeMatrix;
	float4x4 WorldViewProjectionMatrix;
	//Material MaterialObj;
}

//cbuffer cbPerFrame
//{
//	DirectionalLight LightObj;
//	float3 EyePosition;
//}

struct VertexIn
{
	float3 PosL   : POSITION;
    float3 Normal : NORMAL;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VertexOut VS( VertexIn vin )
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), WorldViewProjectionMatrix);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    return vout;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VertexOut pin ) : SV_Target
{
    return pin.Color;
}