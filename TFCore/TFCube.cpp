#include "TFCube.h"
#include "TFVertices.h"
#include "TFUtils.h"
#include <string>

namespace TFCore
{

	TFCube::TFCube()
		:m_pd3dDevice(NULL),
	     m_pDeviceContext(NULL),
		 m_pVertexBuffer(NULL),
		 m_pIndexBuffer(NULL),
		 m_fScale(1.0f),
		 VERTEX_COUNT(8),
		 INDEX_COUNT(36),
		 m_wsShaderPath(L"")
	{
		m_aVertexLayout[0] = TFDefaultSimpleVertexLayout[0];
		m_aVertexLayout[1] = TFDefaultSimpleVertexLayout[1];
	}

	TFCube::TFCube(ID3D11Device* a_pDevice, ID3D11DeviceContext* a_pDeviceContext, float a_fScale)
		:m_pd3dDevice(a_pDevice),
		 m_pDeviceContext(a_pDeviceContext),
		 m_pVertexBuffer(NULL),
		 m_pIndexBuffer(NULL),
		 m_fScale(a_fScale),
		 VERTEX_COUNT(8),
		 INDEX_COUNT(36),
		 m_wsShaderPath(L"")
	{
		m_aVertexLayout[0] = TFDefaultSimpleVertexLayout[0];
		m_aVertexLayout[1] = TFDefaultSimpleVertexLayout[1];
	}


	TFCube::~TFCube()
	{
		ReleaseCOM(m_pVertexBuffer);
	}

	/*** TFIRenderable methods ***/
	const wchar_t* const TFCube::GetShaderPath() const
	{
		return m_wsShaderPath.c_str();
	}

	const D3D11_INPUT_ELEMENT_DESC* const TFCube::GetInputLayout() const
	{
		return m_aVertexLayout;
	}

	void TFCube::SetShaderPath(const std::wstring& a_sFilePath)
	{
		m_wsShaderPath = a_sFilePath;
	}

	void TFCube::Init(ID3D11Device* a_pDevice, ID3D11DeviceContext* a_pDeviceContext, float a_fScale, const std::wstring& a_sFilePath)
	{
		m_pd3dDevice     = a_pDevice;
		m_pDeviceContext = a_pDeviceContext;
		m_fScale         = a_fScale;
		m_wsShaderPath   = a_sFilePath;

		// static vertex data for cube geometry
		TFSimpleVertex vertices[] = 
		{
			{ tfFloat3( -1.0f, 1.0f, -1.0f ), tfFloat4( 0.0f, 0.0f, 1.0f, 1.0f ) },
			{ tfFloat3( 1.0f, 1.0f, -1.0f ),  tfFloat4( 0.0f, 1.0f, 0.0f, 1.0f ) },
			{ tfFloat3( 1.0f, 1.0f, 1.0f ),   tfFloat4( 0.0f, 1.0f, 1.0f, 1.0f ) },
			{ tfFloat3( -1.0f, 1.0f, 1.0f ),  tfFloat4( 1.0f, 0.0f, 0.0f, 1.0f ) },
			{ tfFloat3( -1.0f, -1.0f, -1.0f ),tfFloat4( 1.0f, 0.0f, 1.0f, 1.0f ) },
			{ tfFloat3( 1.0f, -1.0f, -1.0f ), tfFloat4( 1.0f, 1.0f, 0.0f, 1.0f ) },
			{ tfFloat3( 1.0f, -1.0f, 1.0f ),  tfFloat4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ tfFloat3( -1.0f, -1.0f, 1.0f ), tfFloat4( 0.0f, 0.0f, 0.0f, 1.0f ) },
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
		bd.ByteWidth      = sizeof( TFSimpleVertex ) * VERTEX_COUNT;
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
			3,1,0,
			2,1,3,

			0,5,4,
			1,5,0,

			3,4,7,
			0,4,3,

			1,6,5,
			2,6,1,

			2,7,6,
			3,7,2,

			6,4,5,
			7,4,6,
		};

		// describe this index buffer
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof( WORD ) * INDEX_COUNT;
		bd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags      = 0; // Unused
		InitData.pSysMem  = indices;
		HR(m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer));

		// Generate Shaders
		GenerateShaders();
	}

	void TFCube::GenerateShaders()
	{
		// VERTEX SHADER

		// Generate compiled object
		ID3DBlob* _pVSBlob = NULL;
		TFIRenderable::CompileShaderFromFile(m_wsShaderPath.c_str(), "VS", "vs_5_0", &_pVSBlob);

		// Create vertex shader from compiled object
		HR(m_pd3dDevice->CreateVertexShader(_pVSBlob->GetBufferPointer(), _pVSBlob->GetBufferSize(), NULL, &m_pVertexShader));

		// Create the input layout
		size_t _nNumElements = ARRAYSIZE(TFDefaultSimpleVertexLayout);
		HR(m_pd3dDevice->CreateInputLayout(TFDefaultSimpleVertexLayout, _nNumElements, 
			_pVSBlob->GetBufferPointer(), _pVSBlob->GetBufferSize(), &m_pInputLayout));

		// Set the input layout
		//m_pDeviceContext->IASetInputLayout(m_pInputLayout);

		ReleaseCOM(_pVSBlob);

		// PIXEL SHADER
		ID3DBlob* _pPSBlob = NULL;
		CompileShaderFromFile(m_wsShaderPath.c_str(), "PS", "ps_5_0", &_pPSBlob );

		// Create the pixel shader
		HR(m_pd3dDevice->CreatePixelShader( _pPSBlob->GetBufferPointer(), _pPSBlob->GetBufferSize(), NULL, &m_pPixelShader));

		ReleaseCOM(_pPSBlob);
	}

	ID3D11VertexShader* TFCube::GetVertexShader() const
	{
		return m_pVertexShader;
	}

	ID3D11PixelShader*  TFCube::GetPixelShader() const
	{
		return m_pPixelShader;
	}

	// TODO: Its inefficient to be setting all these states per draw call, should 
	// only do it once per batch of renderable types
	void TFCube::Draw()
	{
		// Set the input layout
		m_pDeviceContext->IASetInputLayout(m_pInputLayout);

		// Set vertex buffer
		size_t stride = sizeof( TFSimpleVertex );
		size_t offset = 0;
		m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		// Set index buffer
		m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		// Set primitive topology
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// Draw self
		m_pDeviceContext->DrawIndexed(INDEX_COUNT, 0, 0);
	}
}
