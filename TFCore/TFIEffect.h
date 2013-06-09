#pragma once

#include <d3d11.h>
#include <string>
#include <vector>
#include "TFTypes.h"

namespace TFRendering
{
	class TFIEffect
	{
	public:

		TFIEffect(ID3D11Device* a_pDevice,
			ID3D11DeviceContext*a_pDeviceContext,
			std::wstring& a_wsVertexShaderPath,
			std::wstring& a_wsPixelShaderPath);
		virtual ~TFIEffect();

		void AddRenderable(class TFIRenderable* a_pRenderable);
		virtual void BatchDraw(const tfMatrix& a_matViewProj, 
			const tfMatrix& a_matLightVPT) = 0;

	protected:
		void SetRenderState();

	private:
		// disable copying
		TFIEffect(TFIEffect&);
		TFIEffect& operator=(TFIEffect&);

		void BuildVertexShaderAndInputLayout();
		void BuildPixelShader();

	protected:
		ID3D11Device*			m_pDevice;
		ID3D11DeviceContext*	m_pDeviceContext;
		ID3D11InputLayout*		m_pInputLayout;
		ID3D11VertexShader*		m_pVertexShader;
		ID3D11PixelShader*		m_pPixelShader;
		std::wstring			m_wsShaderPrefix;
		std::wstring			m_wsVertexShaderPath;
		std::wstring			m_wsPixelShaderPath;
		UINT                    m_nStartSlotVB;
		UINT                    m_nNumVertexBuffers;
		ID3D11Buffer**          m_pVertexBuffers;
		std::vector<UINT>       m_vStrides;
		std::vector<UINT>       m_vOffsets;
		ID3D11Buffer*           m_pCBPerObject;

		std::vector<TFIRenderable*> m_vRenderables;
	};
}