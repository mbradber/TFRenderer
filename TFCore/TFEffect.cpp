#include "TFEffect.h"
#include <fstream>
#include "TFUtils.h"
#include <D3Dcompiler.h>

namespace TFRendering
{
	using namespace std;

	TFEffect::TFEffect()
		:m_pDevice(NULL),
		 m_pDeviceContext(NULL),
		 m_pInputLayout(NULL),
		 m_pVertexShader(NULL),
		 m_pPixelShader(NULL)
	{

	}

	TFEffect::~TFEffect()
	{

	}

	void TFEffect::Initialize(ID3D11Device* a_pDevice,
		ID3D11DeviceContext* a_pDeviceContext,
		const std::wstring& a_wsShaderPath,
		const std::wstring& a_wsShaderPrefix)
	{
		m_pDevice = a_pDevice;
		m_pDeviceContext = a_pDeviceContext;
		m_wsShaderPrefix = a_wsShaderPrefix;

		BuildVertexShader(a_wsShaderPath);
		QueryVertexShader();
	}

	void TFEffect::BuildVertexShader(const std::wstring& a_wsShaderPath)
	{
		std::wstring _wsShaderPrefix = m_wsShaderPrefix;
		_wsShaderPrefix.append(a_wsShaderPath);

		// Open compiled shader file and read it...
		ULONG _nFileSize = 0;
		ifstream _reader(_wsShaderPrefix.c_str(), ios::in | ios::binary | ios::ate);
		
		UCHAR* _cbBuffer;
		ID3D11VertexShader* _pVertexShader;

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

		TF_ASSERT(m_pDevice != NULL, FILE_NAME, LINE_NO);

		// Create vertex shader from compiled object
		HR(m_pDevice->CreateVertexShader(_cbBuffer, _nFileSize, NULL, &_pVertexShader));


		// REFLECTION ON VS

		ID3D11ShaderReflection* _pReflection = NULL;
		D3DReflect(_cbBuffer, _nFileSize, IID_ID3D11ShaderReflection, (void**)&_pReflection);

		D3D11_SHADER_DESC _vsDesc;
		_pReflection->GetDesc(&_vsDesc);
		size_t _nInputArgs = _vsDesc.InputParameters;
		size_t _nOutputArgs = _vsDesc.OutputParameters;

		// build input layout
		D3D11_INPUT_ELEMENT_DESC* _pElementDescriptions = new D3D11_INPUT_ELEMENT_DESC[_nInputArgs];

		for(size_t i = 0; i < _nInputArgs; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC _spInputDesc;
			_pReflection->GetInputParameterDesc(i, &_spInputDesc);

			_pElementDescriptions[i].SemanticName = _spInputDesc.SemanticName;
			_pElementDescriptions[i].SemanticIndex = _spInputDesc.SemanticIndex;

			// set the format type for the input layout
			if(_spInputDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				if(_spInputDesc.Mask == 3) // 2 components of 4 component register are being used
				{
					_pElementDescriptions[i].Format = DXGI_FORMAT_R32G32_FLOAT;
				}
				else if(_spInputDesc.Mask == 7) // 3 components are being used
				{
					_pElementDescriptions[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				}
				else if(_spInputDesc.Mask == 15) // all 4 components are being used
				{
					_pElementDescriptions[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				}
			}

			// set the input slots (each vertex component will be in a different vertex buffer)
			LPCSTR _lpsSemanticName = _pElementDescriptions[i].SemanticName;
			if (strcmp(_lpsSemanticName, "POSITION") == 0)
			{
				_pElementDescriptions[i].InputSlot = 0;
			}
			else if(strcmp(_lpsSemanticName, "NORMAL") == 0)
			{
				_pElementDescriptions[i].InputSlot = 1;
			}
			else if(strcmp(_lpsSemanticName, "TEXCOORD") == 0)
			{
				_pElementDescriptions[i].InputSlot = 2;
			}
			else if(strcmp(_lpsSemanticName, "TANGENT") == 0)
			{
				_pElementDescriptions[i].InputSlot = 3;
			}
			else
			{
				// semantic name unsupported
				TF_ASSERT(false, FILE_NAME, LINE_NO);
			}

			_pElementDescriptions[i].AlignedByteOffset = 0;
			_pElementDescriptions[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			_pElementDescriptions[i].InstanceDataStepRate = 0;
		}

		// Create input layout from shader object
		HR(m_pDevice->CreateInputLayout(_pElementDescriptions, _nInputArgs, _cbBuffer, _nFileSize, &m_pInputLayout));

		// Free buffers
		delete[] _cbBuffer;
		delete[] _pElementDescriptions;
	}

	void TFEffect::QueryVertexShader()
	{

	}
}