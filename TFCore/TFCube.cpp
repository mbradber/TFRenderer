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
		 m_pIndexBuffer(NULL),
		 m_pVertexShader(NULL),
		 m_pPixelShader(NULL),
		 m_pInputLayout(NULL),
		 m_pConstantBufferWVP(NULL),
		 m_pConstantBufferLight(NULL),
		 m_fScale(1.0f),
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
		TFPosNormVertex vertices[] = 
		{
			{ XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT3( -NORMALIZED_COMPONENT,  NORMALIZED_COMPONENT, -NORMALIZED_COMPONENT) },  // front, top-left
			{ XMFLOAT3( 1.0f, 1.0f, -1.0f ),  XMFLOAT3(  NORMALIZED_COMPONENT,  NORMALIZED_COMPONENT, -NORMALIZED_COMPONENT) },  // front, top-right
			{ XMFLOAT3( 1.0f, 1.0f, 1.0f ),   XMFLOAT3(  NORMALIZED_COMPONENT,  NORMALIZED_COMPONENT,  NORMALIZED_COMPONENT) },  // back, top-right
			{ XMFLOAT3( -1.0f, 1.0f, 1.0f ),  XMFLOAT3( -NORMALIZED_COMPONENT,  NORMALIZED_COMPONENT,  NORMALIZED_COMPONENT) },  // back, top-left
			{ XMFLOAT3( -1.0f, -1.0f, -1.0f ),XMFLOAT3( -NORMALIZED_COMPONENT, -NORMALIZED_COMPONENT, -NORMALIZED_COMPONENT) },  // front, bottom-left
			{ XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT3(  NORMALIZED_COMPONENT, -NORMALIZED_COMPONENT, -NORMALIZED_COMPONENT) },  // front, bottom-righ
			{ XMFLOAT3( 1.0f, -1.0f, 1.0f ),  XMFLOAT3(  NORMALIZED_COMPONENT, -NORMALIZED_COMPONENT,  NORMALIZED_COMPONENT) },  // back, bottom-right
			{ XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT3( -NORMALIZED_COMPONENT, -NORMALIZED_COMPONENT,  NORMALIZED_COMPONENT) },  // back, bottom-left
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
		bd.ByteWidth      = sizeof( TFPosNormVertex ) * VERTEX_COUNT;
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
		// Generate resources that will be used with the shaders for this object
		GenerateShaderResources();
	}

	// TODO: Should pass a D3D11_INPUT_ELEMENT_DESC* to this function so we don't have to copy/paste the 
	// array twice everytime we change it, this will be the source of a bug in the future...
	void TFCube::GenerateShaders()
	{
		// VERTEX SHADER

		// Generate compiled object
		ID3DBlob* _pVSBlob = NULL;
		CompileShaderFromFile(m_wsShaderPath.c_str(), "VS", "vs_5_0", &_pVSBlob);

		// Create vertex shader from compiled object
		HR(m_pd3dDevice->CreateVertexShader(_pVSBlob->GetBufferPointer(), _pVSBlob->GetBufferSize(), NULL, &m_pVertexShader));

		// Create the input layout
		size_t _nNumElements = ARRAYSIZE(TFPositionNormalVertexLayout);
		HR(m_pd3dDevice->CreateInputLayout(TFPositionNormalVertexLayout, _nNumElements, 
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

	void TFCube::CompileShaderFromFile(const wchar_t* a_cbFileName, LPCSTR a_pEntryPoint, LPCSTR a_pShaderModel, ID3DBlob** a_ppBlobOut)
	{
		DWORD _dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
		_dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

		ID3DBlob* _pErrorBlob = NULL;
		HRESULT _hr = D3DX11CompileFromFile(a_cbFileName, NULL, NULL, a_pEntryPoint, a_pShaderModel, 
			_dwShaderFlags, 0, NULL, a_ppBlobOut, &_pErrorBlob, NULL);

		// Output debug info if compilation failed
		if(FAILED(_hr))
		{
			if(_pErrorBlob != NULL)
			{
				OutputDebugStringA((char*)_pErrorBlob->GetBufferPointer());
			}

			HR(_hr);
		}

		ReleaseCOM(_pErrorBlob);
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
	void TFCube::UpdateResources(const XMMATRIX& a_matWVP, const XMMATRIX& a_matWorld, const TFMaterial& a_material)
	{
		TFCore::TFBufferWVP cb;
		cb.wvpMatrix = XMMatrixTranspose(a_matWVP);
		m_pDeviceContext->UpdateSubresource(m_pConstantBufferWVP , 0, NULL, &cb, 0, 0);
	}

	void TFCube::ActivateShaders()
	{
		m_pDeviceContext->VSSetShader(GetVertexShader(), NULL, 0);
		m_pDeviceContext->PSSetShader(GetPixelShader(), NULL, 0);
		m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

		// Set the input layout
		m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	}

	// TODO: Its inefficient to be setting all these states per draw call, should 
	// only do it once per batch of renderable types
	void TFCube::Draw()
	{
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
