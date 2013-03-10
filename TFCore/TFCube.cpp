#include "TFCube.h"
#include "TFVertices.h"
#include "TFUtils.h"
#include <string>
#include <d3dcompiler.h>

#define NORMALIZED_COMPONENT 0.57735026918962576450914878050196f

namespace TFCore
{

	TFCube::TFCube()
		:m_pd3dDevice(NULL),
	     m_pDeviceContext(NULL),
		 m_pVertexBuffer(NULL),
		 m_pVertexBufferTexCoords(NULL),
		 m_pIndexBuffer(NULL),
		 m_pVertexShader(NULL),
		 m_pPixelShader(NULL),
		 m_pInputLayout(NULL),
		 m_pConstantBufferWVP(NULL),
		 m_pConstantBufferLight(NULL),
		 m_fScale(1.0f),
		 VERTEX_COUNT(24),
		 INDEX_COUNT(36),
		 m_wsShaderPath(L"")
	{
	}

	TFCube::~TFCube()
	{
		ReleaseCOM(m_pVertexBuffer);
		ReleaseCOM(m_pIndexBuffer);
		ReleaseCOM(m_pVertexShader);
		ReleaseCOM(m_pPixelShader);
		ReleaseCOM(m_pConstantBufferLight);
		ReleaseCOM(m_pConstantBufferWVP);
	}

	/*** TFIRenderable methods ***/
	const wchar_t* const TFCube::GetShaderPath() const
	{
		return m_wsShaderPath.c_str();
	}

	void TFCube::SetShaderPath(const std::wstring& a_sFilePath)
	{
		m_wsShaderPath = a_sFilePath;
	}

	void TFCube::Init(ID3D11Device* a_pDevice, 
		ID3D11DeviceContext* a_pDeviceContext, 
		float a_fScale, 
		ID3D11VertexShader* a_pVertexShader,
		ID3D11PixelShader* a_pPixelShader,
		ID3D11InputLayout* a_pInputLayout,
		const std::wstring& a_sFilePathTexture)
	{
		m_pd3dDevice     = a_pDevice;
		m_pDeviceContext = a_pDeviceContext;
		m_fScale         = a_fScale;
		m_wsTexturePath  = a_sFilePathTexture;
		m_pVertexShader  = a_pVertexShader;
		m_pPixelShader   = a_pPixelShader;
		m_pInputLayout   = a_pInputLayout;


		// Fill in the front face vertex data.
		TFPosNormTex vertices[] = 
		{
	        {XMFLOAT3(-1, -1, -1), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2( 0.0f, 1.0f)},
	        {XMFLOAT3(-1, +1, -1), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2( 0.0f, 0.0f)},
	        {XMFLOAT3(+1, +1, -1), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2( 1.0f, 0.0f)},
	        {XMFLOAT3(+1, -1, -1), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2( 1.0f, 1.0f)},

			{XMFLOAT3(-1, -1, +1), XMFLOAT3(0.0f, 0.0f, 1.0f),  XMFLOAT2( 1.0f, 1.0f)},
	        {XMFLOAT3(+1, -1, +1), XMFLOAT3(0.0f, 0.0f, 1.0f),  XMFLOAT2( 0.0f, 1.0f)},
	        {XMFLOAT3(+1, +1, +1), XMFLOAT3(0.0f, 0.0f, 1.0f),  XMFLOAT2( 0.0f, 0.0f)},
	        {XMFLOAT3(-1, +1, +1), XMFLOAT3(0.0f, 0.0f, 1.0f),  XMFLOAT2( 1.0f, 0.0f)},

			{XMFLOAT3(-1, +1, -1), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2( 0.0f, 1.0f)},
	        {XMFLOAT3(-1, +1, +1), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2( 0.0f, 0.0f)},
	        {XMFLOAT3(+1, +1, +1), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2( 1.0f, 0.0f)},
	        {XMFLOAT3(+1, +1, -1), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2( 1.0f, 1.0f)},

			{XMFLOAT3(-1, -1, -1), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2( 1.0f, 1.0f)},
	        {XMFLOAT3(+1, -1, -1), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2( 0.0f, 1.0f)},
	        {XMFLOAT3(+1, -1, +1), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2( 0.0f, 0.0f)},
	        {XMFLOAT3(-1, -1, +1), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2( 1.0f, 0.0f)},

			{XMFLOAT3(-1, -1, +1), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2( 0.0f, 1.0f)},
	        {XMFLOAT3(-1, +1, +1), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2( 0.0f, 0.0f)},
	        {XMFLOAT3(-1, +1, -1), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2( 1.0f, 0.0f)},
	        {XMFLOAT3(-1, -1, -1), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2( 1.0f, 1.0f)},

			{XMFLOAT3(+1, -1, -1), XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(0.0f, 1.0f)},
	        {XMFLOAT3(+1, +1, -1), XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(0.0f, 0.0f)},
	        {XMFLOAT3(+1, +1, +1), XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(1.0f, 0.0f)},
			{XMFLOAT3(+1, -1, +1), XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(1.0f, 1.0f)},
		};

		// scale the geometry
		for(size_t i = 0; i < VERTEX_COUNT; ++i)
		{
			vertices[i].Pos.x *= a_fScale;
			vertices[i].Pos.y *= a_fScale;
			vertices[i].Pos.z *= a_fScale;
		}

		// describe this buffer
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof( TFPosNormTex ) * VERTEX_COUNT;
		bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0; // No cpu access
		bd.MiscFlags      = 0; // Unused

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory( &InitData, sizeof(InitData) );
		InitData.pSysMem = vertices;

		// Create a buffer to hold this cube's vert data in video memory
		HR(m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer));

		// Create index buffer
		WORD indices[] =
		{
			// front face
			0,1,2,
			0,2,3,

			// back face
			4,5,6,
			4,6,7,

			// top face
			8,9,10,
			8,10,11,

			// bottom face
			12,13,14,
			12,14,15,

			// left face
			16,17,18,
			16,18,19,

			// right face
			20,21,22,
			20,22,23
		};

		// describe this index buffer
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof( WORD ) * INDEX_COUNT;
		bd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags      = 0; // Unused
		InitData.pSysMem  = indices;
		HR(m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer));

		// Generate resources that will be used with the shaders for this object
		GenerateShaderResources();
	}

	// TODO: Look into making this a more general function or moving it into a "Resource Manager". It is not 
	// good practice to have to replace the type everytime we want to use a new vertex type.
	void TFCube::GenerateShaderResources()
	{
		// describe the cb for the WVP matrix
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof(TFBufferWVP);
		bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags      = 0;

		// Create the constant buffer with the device
		HR(m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pConstantBufferWVP));

		// describe the cb for the directional light
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof(TFBufferDirectionalLight);
		bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags      = 0;

		// Create the constant buffer with the device
		HR(m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pConstantBufferLight));

		// TODO: D3DX stuff is deprecated, use another method for loading textures when you have time.

		// Create the texture for this box
		HR(D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, m_wsTexturePath.c_str(), NULL, NULL, &m_pCrateTextureSRV, NULL));
	}

	ID3D11VertexShader* TFCube::GetVertexShader() const
	{
		return m_pVertexShader;
	}

	ID3D11PixelShader*  TFCube::GetPixelShader() const
	{
		return m_pPixelShader;
	}

	// TODO: Address tedious type changing issue...
	void TFCube::UpdateResources(const XMMATRIX& a_matWVP, const XMMATRIX& a_matWorld, const XMFLOAT3& a_vEyePos)
	{
		//UPDATE TRANSFORM RESOURCE

		TFCore::TFBufferWVP cb;

		// update world matrix
		cb.worldMatrix = XMMatrixTranspose(a_matWorld);

		// Update world inverse transpose matrix (used to transform normals as it will be distorted 
		// with non uniform scaling transforms, see pg. 277 of Luna...
		XMMATRIX _wit = a_matWorld;
		_wit.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR _witDet = XMMatrixDeterminant(_wit);

		cb.worldInvTransposeMatrix = XMMatrixInverse(&_witDet, _wit);

		// update wvp of buffer
		cb.wvpMatrix = XMMatrixTranspose(a_matWVP);

		//update material of buffer
		cb.material  = m_material;

		m_pDeviceContext->UpdateSubresource(m_pConstantBufferWVP , 0, NULL, &cb, 0, 0);

		// UPDATE LIGHT RESOURCE
		TFBufferDirectionalLight cbLights;
		cbLights.DirLight = m_dirLight;
		cbLights.EyePos   = a_vEyePos;

		m_pDeviceContext->UpdateSubresource(m_pConstantBufferLight, 0, NULL, &cbLights, 0, 0);
	}

	// TODO: The slot number argument that is specified when binding these resources corresponds to 
	// the register number used in the shader. This is prone to shader errors and the HLSL reflection API should
	// be used to try and get the name of resources in the shader instead.
	void TFCube::ActivateShaders()
	{
		// Bind shaders
		m_pDeviceContext->VSSetShader(GetVertexShader(), NULL, 0);
		m_pDeviceContext->PSSetShader(GetPixelShader(), NULL, 0);
		// Bind constant buffers to VS and PS
		m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBufferWVP);
		m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pConstantBufferLight);
		m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pConstantBufferLight);
		m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBufferWVP);
		// Bind texture
		m_pDeviceContext->PSSetShaderResources(0, 1, &m_pCrateTextureSRV);

		// Set the input layout
		m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	}

	// TODO: Its inefficient to be setting all these states per draw call, should 
	// only do it once per batch of renderable types
	void TFCube::Draw()
	{
		// Set vertex buffers
		UINT _nStride = sizeof(TFPosNormTex);
		UINT _nOffset = 0;

		m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &_nStride, &_nOffset);

		// Set index buffer
		m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		// Set primitive topology
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// Draw self
		m_pDeviceContext->DrawIndexed(INDEX_COUNT, 0, 0);
	}
}
