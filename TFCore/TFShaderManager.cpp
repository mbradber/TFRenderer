#include "TFShaderManager.h"

#include "TFUtils.h"
#include <fstream>

using namespace std;

namespace TFCore
{

	TFShaderManager::TFShaderManager()
		:m_pd3dDevice(NULL),
		 m_pActiveVertexShader(NULL),
		 m_pActivePixelShader(NULL),
		 m_pActiveInputLayout(NULL),
		 m_pSamplerStateAniso(NULL)
	{
	}

	TFShaderManager::~TFShaderManager()
	{
		ReleaseCOM(m_pActiveVertexShader);
		ReleaseCOM(m_pActivePixelShader);
	}

	void TFShaderManager::Init(ID3D11Device* a_pDevice)
	{
		m_pd3dDevice = a_pDevice;
	}

	void TFShaderManager::SetActiveVertexShader(const std::wstring& a_sFilePathShader, const D3D11_INPUT_ELEMENT_DESC* a_inputLayout, size_t a_nComponents)
	{
		// Open compiled shader file and read it...
		ULONG _nFileSize = 0;
		ifstream _reader(a_sFilePathShader.c_str(), ios::in | ios::binary | ios::ate);
		UCHAR* _cbBuffer;

		if(_reader.is_open())
		{
			_nFileSize = static_cast<ULONG>(_reader.tellg());
			_cbBuffer  = new UCHAR[_nFileSize];
		}
		else
		{
			// File could not be opened
			TF_ASSERT(false, FILE_NAME, LINE_NO);
		}

		// Load the file into memory
		_reader.seekg(0, ios::beg); // return the get pointer to beginning of file
		_reader.read(reinterpret_cast<char*>(_cbBuffer), _nFileSize);

		if(_reader.is_open())
		{
			_reader.close();
		}

		TF_ASSERT(m_pd3dDevice != NULL, FILE_NAME, LINE_NO);

		// Create vertex shader from compiled object
		HR(m_pd3dDevice->CreateVertexShader(_cbBuffer, _nFileSize, NULL, &m_pActiveVertexShader));	

		// Create input layout from shader object
		HR(m_pd3dDevice->CreateInputLayout(a_inputLayout, a_nComponents, _cbBuffer, _nFileSize, &m_pActiveInputLayout));

		// Free buffer
		delete[] _cbBuffer;
	}

	void TFShaderManager::SetActivePixelShader(const std::wstring& a_sFilePathShader)
	{
		// Open compiled shader file and read it...
		ULONG _nFileSize = 0;
		ifstream _reader(a_sFilePathShader.c_str(), ios::in | ios::binary | ios::ate);
		UCHAR* _cbBuffer;

		if(_reader.is_open())
		{
			_nFileSize = static_cast<ULONG>(_reader.tellg());
			_cbBuffer  = new UCHAR[_nFileSize];
		}
		else
		{
			// File could not be opened
			TF_ASSERT(false, FILE_NAME, LINE_NO);
		}

		// Load the file into memory
		_reader.seekg(0, ios::beg); // return the get pointer to beginning of file
		_reader.read(reinterpret_cast<char*>(_cbBuffer), _nFileSize);

		if(_reader.is_open())
		{
			_reader.close();
		}

		TF_ASSERT(m_pd3dDevice != NULL, FILE_NAME, LINE_NO);
		// Create pixel shader
		HR(m_pd3dDevice->CreatePixelShader(_cbBuffer, _nFileSize, NULL, &m_pActivePixelShader));

		// Free buffer
		delete[] _cbBuffer;

		BuildSamplerStates();
	}

	void TFShaderManager::BuildSamplerStates()
	{
		D3D11_SAMPLER_DESC _anisoSampler;
		_anisoSampler.Filter		= D3D11_FILTER_ANISOTROPIC;
		_anisoSampler.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
		_anisoSampler.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
		_anisoSampler.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
		_anisoSampler.MinLOD		= -FLT_MAX;
		_anisoSampler.MaxLOD		= FLT_MAX;
		_anisoSampler.MipLODBias    = 0;
		_anisoSampler.MaxAnisotropy = 16;
		_anisoSampler.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

		HR(m_pd3dDevice->CreateSamplerState(&_anisoSampler, &m_pSamplerStateAniso));
	}

	ID3D11SamplerState* TFShaderManager::GetSamplerState() const
	{
		return m_pSamplerStateAniso;
	}

	ID3D11VertexShader* TFShaderManager::GetActiveVertexShader() const
	{
		return m_pActiveVertexShader;
	}

	ID3D11PixelShader* TFShaderManager::GetActivePixelShader() const
	{
		return m_pActivePixelShader;
	}

	ID3D11InputLayout* TFShaderManager::GetActiveInputLayout() const
	{
		return m_pActiveInputLayout;
	}

}

