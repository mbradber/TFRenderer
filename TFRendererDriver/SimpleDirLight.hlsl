


struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float  _pad;
};

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 WorldMatrix;
	float4x4 WorldInverseTransposeMatrix;
	float4x4 WorldViewProjectionMatrix;
	Material MaterialObj;
}

cbuffer cbPerFrame : register(b1)
{
	DirectionalLight LightObj;
	float3 EyePosition;
}

Texture2D DiffuseMap : register(t0);

SamplerState samAnisotropic
{
	Filter=ANISOTROPIC;
	MaxAnisotropy=4;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
	float3 PosL   : POSITION;
    float3 Normal : NORMAL;
	float2 TexC   : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 PosW  : POSITION;
	float4 NormW : NORMAL;
	float2 TexC  : TEXCOORD;
};


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