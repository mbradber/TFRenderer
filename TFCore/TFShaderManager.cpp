#include "TFShaderManager.h"
#include "TFUtils.h"
#include "TFVertices.h"
#include <d3dcompiler.h>

namespace TFCore
{

	TFShaderManager::TFShaderManager()
		:m_pd3dDevice(NULL),
		 m_pDeviceContext(NULL),
		 m_pVertexShader(NULL),
		 m_pInputLayout(NULL)
	{

	}

	TFShaderManager::~TFShaderManager()
	{

	}

	void TFShaderManager::Init(ID3D11Device* a_pd3dDevice)
	{
		m_pd3dDevice = a_pd3dDevice;
	}

	void TFShaderManager::CompileShaderFromFile(WCHAR* a_cbFileName, LPCSTR a_pEntryPoint, LPCSTR a_pShaderModel, ID3DBlob** a_ppBlobOut)
	{
		DWORD _dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	#if defined( DEBUG ) || defined( _DEBUG )
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
		}

		ReleaseCOM(_pErrorBlob);
	}

	void TFShaderManager::GenerateDefaultPosColVertexShader()
	{
		// Generate compiled object
		ID3DBlob* _pVSBlob = NULL;
		CompileShaderFromFile(L"SimplePosCol.hlsl", "VS", "vs_4_0", &_pVSBlob);

		// Create vertex shader from compiled object
		HR(m_pd3dDevice->CreateVertexShader(_pVSBlob->GetBufferPointer(), _pVSBlob->GetBufferSize(), NULL, &m_pVertexShader));

		// Create the input layout
		size_t _nNumElements = ARRAYSIZE(TFDefaultSimpleVertexLayout);
		HR(m_pd3dDevice->CreateInputLayout(TFDefaultSimpleVertexLayout, _nNumElements, 
			_pVSBlob->GetBufferPointer(), _pVSBlob->GetBufferSize(), &m_pInputLayout));

		// Set the input layout
		m_pDeviceContext->IASetInputLayout(m_pInputLayout);

		ReleaseCOM(_pVSBlob);
	}

	void TFShaderManager::GenerateDefaultPosColPixelShader()
	{
		ID3DBlob* _pPSBlob = NULL;
		CompileShaderFromFile( L"SimplePosCol.hlsl", "PS", "ps_4_0", &_pPSBlob );

		// Create the pixel shader
		HR(m_pd3dDevice->CreatePixelShader( _pPSBlob->GetBufferPointer(), _pPSBlob->GetBufferSize(), NULL, &m_pPixelShader));

		ReleaseCOM(_pPSBlob);
	}

	// TODO: this should be in the resource manager
	void TFShaderManager::GenerateDefaultPosColWVPBuffer()
	{
		// describe the cb
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof(TFBufferWVP);
		bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags      = 0;

		// Create the constant buffer with the device
		HR(m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pConstantBuffer));
	}

}
