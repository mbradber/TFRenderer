#pragma once

#include <string>
#include <d3d11.h>

namespace TFCore
{

	class TFShaderManager
	{
	public:
		TFShaderManager();
		~TFShaderManager();

		void Init(ID3D11Device* a_pDevice);

		void SetActiveVertexShader(const std::wstring& a_sFilePathShader, const D3D11_INPUT_ELEMENT_DESC* a_inputLayout, size_t a_nComponents);
		void SetActivePixelShader(const std::wstring& a_sFilePathShader);
		ID3D11VertexShader* GetActiveVertexShader() const;
		ID3D11PixelShader*  GetActivePixelShader() const;
		ID3D11InputLayout*  GetActiveInputLayout() const;

	private:
		ID3D11Device*	    m_pd3dDevice;
		ID3D11VertexShader* m_pActiveVertexShader;
		ID3D11PixelShader*  m_pActivePixelShader;
		ID3D11InputLayout*  m_pActiveInputLayout;
	};

}

