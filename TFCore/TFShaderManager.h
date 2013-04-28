#pragma once

#include <string>
#include <d3d11.h>
#include <map>
#include <vector>

namespace TFCore
{

	enum TFSamplerIndex
	{
		TF_SAMPLER_NONE,
		TF_SAMPLER_ANISOTROPIC,
		TF_SAMPLER_TRILINEAR,
		TF_SAMPLER_POINT,
		TF_SAMPLER_END
	};

	class TFShaderManager
	{
	public:
		TFShaderManager();
		~TFShaderManager();

		void Init(ID3D11Device* a_pDevice);

		void SetActiveVertexShader(const std::wstring& a_sFilePathShader, const D3D11_INPUT_ELEMENT_DESC* a_inputLayout, size_t a_nComponents);
		void SetActivePixelShader(const std::wstring& a_sFilePathShader);
		void AddVertexShader(const std::wstring& a_sName, const std::wstring& a_sFilePathShader, const D3D11_INPUT_ELEMENT_DESC* a_inputLayout, size_t a_nComponents);
		void AddPixelShader(const std::wstring& a_sName, const std::wstring& a_sFilePathShader);
		ID3D11VertexShader* GetSkyVS() { return m_pSkyVertexShader; }
		ID3D11PixelShader* GetSkyPS() { return m_pSkyPixelShader; }

		ID3D11VertexShader* GetVertexShaderByName(const std::wstring& a_sName);
		ID3D11PixelShader*  GetPixelShaderByName(const std::wstring& a_sName);
		ID3D11InputLayout*  GetInputLayoutByName(const std::wstring& a_sName);
		ID3D11VertexShader* GetActiveVertexShader() const;
		ID3D11PixelShader*  GetActivePixelShader() const;
		ID3D11InputLayout*  GetActiveInputLayout() const;
		ID3D11InputLayout*  GetSimplePosInputLayout() const { return m_pSimplePosInputLayout; }
		ID3D11SamplerState* GetSamplerState(TFSamplerIndex a_nSamplerIndex) const;

	private:

		void BuildSamplerStates();

		ID3D11Device*	    m_pd3dDevice;
		ID3D11VertexShader* m_pActiveVertexShader;
		ID3D11VertexShader* m_pSkyVertexShader;
		ID3D11PixelShader*  m_pActivePixelShader;
		ID3D11PixelShader*  m_pSkyPixelShader;
		ID3D11InputLayout*  m_pActiveInputLayout;
		ID3D11InputLayout*  m_pSimplePosInputLayout;
		ID3D11SamplerState* m_pSamplerStateAniso;
		ID3D11SamplerState* m_pSamplerStateTriLinear;

		std::map<std::wstring, ID3D11VertexShader*> m_mapVertexShaders;
		std::map<std::wstring, ID3D11PixelShader*>  m_mapPixelShaders;
		std::map<std::wstring, ID3D11InputLayout*>  m_mapInputLayouts;
		std::vector<ID3D11SamplerState*>            m_vSamplers;
	};

}

