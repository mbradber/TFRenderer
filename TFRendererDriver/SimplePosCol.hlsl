//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
//cbuffer ConstantBuffer : register( b0 )
//{
//	matrix World;
//	matrix View;
//	matrix Projection; 
//}

cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
}

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

////--------------------------------------------------------------------------------------
//struct VS_OUTPUT
//{
//    float4 Pos : SV_POSITION;
//    float4 Color : COLOR0;
//};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VertexOut VS( VertexIn vin )
{
    //VS_OUTPUT output = (VS_OUTPUT)0;
    //output.Pos = mul( Pos, World );
    //output.Pos = mul( output.Pos, View );
    //output.Pos = mul( output.Pos, Projection );
    //output.Color = Color;
    //return output;

	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
    
    return vout;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VertexOut pin ) : SV_Target
{
    return pin.Color;
}

//technique11 SimpleShader
//{
//	pass P0
//	{
//		SetVertexShader(CompileShader(vs_4_0, VS()));
//		SetPixelShader(CompileShader(vs_4_0, PS()));
//	}
//}