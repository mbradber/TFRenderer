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
		 m_pSamplerStateAniso(NULL),
		 m_pSamplerStateTriLinear(NULL),
		 m_wsShaderPrefix(L"")
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

		BuildSamplerStates();
	}

	void TFShaderManager::SetShaderPrefix(const std::wstring& a_wsShaderPrefix)
	{
		m_wsShaderPrefix = a_wsShaderPrefix;
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

	void TFShaderManager::AddVertexShader(const std::wstring& a_sName, 
		const std::wstring& a_sFilePathShader, 
		const D3D11_INPUT_ELEMENT_DESC* a_inputLayout, 
		size_t a_nComponents)
	{
		std::wstring _wsShaderPrefix = m_wsShaderPrefix;
		_wsShaderPrefix.append(a_sFilePathShader);

		// Open compiled shader file and read it...
		ULONG _nFileSize = 0;
		ifstream _reader(_wsShaderPrefix.c_str(), ios::in | ios::binary | ios::ate);
		UCHAR* _cbBuffer;
		ID3D11VertexShader* _pVertexShader;
		ID3D11InputLayout*  _pInputLayout;

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
		HR(m_pd3dDevice->CreateVertexShader(_cbBuffer, _nFileSize, NULL, &_pVertexShader));	

		// Create input layout from shader object
		HR(m_pd3dDevice->CreateInputLayout(a_inputLayout, a_nComponents, _cbBuffer, _nFileSize, &_pInputLayout));

		// Insert generated vertex shader into vertex shader map
		m_mapVertexShaders.insert(std::pair<std::wstring, ID3D11VertexShader*>(a_sName, _pVertexShader));
		// Insert generated input layout into input layout map
		m_mapInputLayouts.insert(std::pair<std::wstring, ID3D11InputLayout*>(a_sName, _pInputLayout));

		// Free buffer
		delete[] _cbBuffer;
	}

	void TFShaderManager::AddPixelShader(const std::wstring& a_sName, const std::wstring& a_sFilePathShader)
	{
		std::wstring _wsShaderPrefix = m_wsShaderPrefix;
		_wsShaderPrefix.append(a_sFilePathShader);

		// Open compiled shader file and read it...
		ULONG _nFileSize = 0;
		ifstream _reader(_wsShaderPrefix.c_str(), ios::in | ios::binary | ios::ate);
		UCHAR* _cbBuffer;
		ID3D11PixelShader* _pPixelShader;

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
		HR(m_pd3dDevice->CreatePixelShader(_cbBuffer, _nFileSize, NULL, &_pPixelShader));

		// insert generated pixel shader into map
		m_mapPixelShaders.insert(std::pair<std::wstring, ID3D11PixelShader*>(a_sName, _pPixelShader));

		// Free buffer
		delete[] _cbBuffer;
	}

	ID3D11VertexShader* TFShaderManager::GetVertexShaderByName(const std::wstring& a_sName)
	{
		return m_mapVertexShaders.find(a_sName)->second;
	}

	ID3D11PixelShader*  TFShaderManager::GetPixelShaderByName(const std::wstring& a_sName)
	{
		return m_mapPixelShaders.find(a_sName)->second;
	}

	ID3D11InputLayout*  TFShaderManager::GetInputLayoutByName(const std::wstring& a_sName)
	{
		return m_mapInputLayouts.find(a_sName)->second;
	}

	// Note: The order that these are inserted matters as the 'TFSamplerIndex' enum expects
	// this.
	void TFShaderManager::BuildSamplerStates()
	{
		ID3D11SamplerState* _pSamplerState = NULL;

		D3D11_SAMPLER_DESC _anisoSampler;
		_anisoSampler.Filter		 = D3D11_FILTER_ANISOTROPIC;
		_anisoSampler.AddressU		 = D3D11_TEXTURE_ADDRESS_WRAP;
		_anisoSampler.AddressV		 = D3D11_TEXTURE_ADDRESS_WRAP;
		_anisoSampler.AddressW		 = D3D11_TEXTURE_ADDRESS_WRAP;
		_anisoSampler.MinLOD		 = -FLT_MAX;
		_anisoSampler.MaxLOD		 = FLT_MAX;
		_anisoSampler.MipLODBias     = 0;
		_anisoSampler.MaxAnisotropy  = 16;
		_anisoSampler.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

		HR(m_pd3dDevice->CreateSamplerState(&_anisoSampler, &_pSamplerState));
		m_vSamplers.push_back(_pSamplerState);

		D3D11_SAMPLER_DESC _triLinearSampler;
		_triLinearSampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		_triLinearSampler.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
		_triLinearSampler.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
		_triLinearSampler.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
		_triLinearSampler.MinLOD		 = -FLT_MAX;
		_triLinearSampler.MaxLOD		 = FLT_MAX;
		_triLinearSampler.MipLODBias     = 0;
		_triLinearSampler.MaxAnisotropy  = 16;
		_triLinearSampler.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

		HR(m_pd3dDevice->CreateSamplerState(&_triLinearSampler, &_pSamplerState));
		m_vSamplers.push_back(_pSamplerState);

		D3D11_SAMPLER_DESC _pointSampler;
		_pointSampler.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		_pointSampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		_pointSampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		_pointSampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		_pointSampler.MinLOD		 = -FLT_MAX;
		_pointSampler.MaxLOD		 = FLT_MAX;
		_pointSampler.MipLODBias     = 0;
		_pointSampler.MaxAnisotropy  = 16;
		_pointSampler.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

		HR(m_pd3dDevice->CreateSamplerState(&_pointSampler, &_pSamplerState));
		m_vSamplers.push_back(_pSamplerState);

        D3D11_SAMPLER_DESC SamDescShad = 
        {
            D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,// D3D11_FILTER Filter;
            D3D11_TEXTURE_ADDRESS_BORDER, //D3D11_TEXTURE_ADDRESS_MODE AddressU;
            D3D11_TEXTURE_ADDRESS_BORDER, //D3D11_TEXTURE_ADDRESS_MODE AddressV;
            D3D11_TEXTURE_ADDRESS_BORDER, //D3D11_TEXTURE_ADDRESS_MODE AddressW;
            0,//FLOAT MipLODBias;
            0,//UINT MaxAnisotropy;
            D3D11_COMPARISON_LESS , //D3D11_COMPARISON_FUNC ComparisonFunc;
            0.0,0.0,0.0,0.0,//FLOAT BorderColor[ 4 ];
            0,//FLOAT MinLOD;
            0//FLOAT MaxLOD;   
        };

		HR(m_pd3dDevice->CreateSamplerState(&SamDescShad, &_pSamplerState));
		m_vSamplers.push_back(_pSamplerState);
	}

	ID3D11SamplerState* TFShaderManager::GetSamplerState(TFSamplerIndex a_nSamplerIndex) const
	{
		// subtract one to account for the 'none' slot
		return m_vSamplers[a_nSamplerIndex - 1];
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

