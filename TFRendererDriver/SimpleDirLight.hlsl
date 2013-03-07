


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

struct VertexIn
{
	float3 PosL   : POSITION;
    float3 Normal : NORMAL;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 PosW  : POSITION;
	float4 NormW : NORMAL;
};


VertexOut VS( VertexIn vin )
{
	VertexOut vout;
	
	// Execute transformations
	vout.PosH  = mul(float4(vin.PosL, 1.0f), WorldViewProjectionMatrix);
	vout.PosW  = mul(float4(vin.PosL, 1.0f), WorldMatrix);
	vout.NormW = mul(float4(vin.Normal, 0.0f), WorldInverseTransposeMatrix);
    
    return vout;
}

float4 PS( VertexOut pin ) : SV_Target
{
	// Renormalize after transformation
	pin.NormW = normalize(pin.NormW);

	float4 _lightVec     = float4(LightObj.Direction, 0.0f) * -1.0f;
	float  _lambert      = max(0, dot(_lightVec, pin.NormW));
	float4 _diffuseLight = _lambert * LightObj.Diffuse * MaterialObj.Diffuse;
	float4 _ambientLight = LightObj.Ambient * MaterialObj.Ambient;
	float4 _color        = _ambientLight + _diffuseLight;

	return _color;
}