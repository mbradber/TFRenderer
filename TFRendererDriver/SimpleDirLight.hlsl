


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
    float4 Color : COLOR;
};


VertexOut VS( VertexIn vin )
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), WorldViewProjectionMatrix);

	// Calculate the normal vector and light vector in world space
	float4 _normalWorld = mul(float4(vin.Normal, 0.0f), WorldInverseTransposeMatrix);
	float4 _posWorld    = mul(float4(vin.PosL, 1.0f), WorldMatrix);
	//float4 _lightVec    = float4(EyePosition, 1.0f) - _posWorld;
	float4 _lightVec = float4(LightObj.Direction, 0.0f) * -1; // Reverse the light direction for the dot product

	// Calculate the diffuse light using Lambert's 
	float _lambert = max(0, dot(_lightVec, _normalWorld)); // determines intensity of the light
	float4 _diffuseLight = _lambert * LightObj.Diffuse;
	_diffuseLight = _diffuseLight * MaterialObj.Diffuse; // component multiplication with the material's diffuse comp

	// Calculate the ambient light
	float4 _ambientLight = LightObj.Ambient * MaterialObj.Ambient; // component multiplication with the material's ambient comp
	
	// Just pass vertex color into the pixel shader.
    vout.Color = _ambientLight + _diffuseLight;
    
    return vout;
}

float4 PS( VertexOut pin ) : SV_Target
{
    return pin.Color;
}