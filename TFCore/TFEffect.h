#pragma once

#include <d3d11.h>
#include <string>

namespace TFRendering
{
	class TFEffect
	{
	public:

		TFEffect();
		~TFEffect();

		void Initialize(ID3D11Device* a_pDevice,
			ID3D11DeviceContext* a_pDeviceContext,
			const std::wstring& a_wsShaderPath,
			const std::wstring& a_wsShaderPrefix);

	private:
		// no copying
		TFEffect(TFEffect&);
		TFEffect& operator=(TFEffect&);

		void BuildVertexShader(const std::wstring& a_wsShaderPath);
		void QueryVertexShader();

	private:
		ID3D11Device*			m_pDevice;
		ID3D11DeviceContext*	m_pDeviceContext;
		ID3D11InputLayout*		m_pInputLayout;
		ID3D11VertexShader*		m_pVertexShader;
		ID3D11PixelShader*		m_pPixelShader;
		std::wstring			m_wsShaderPrefix;
	};
}