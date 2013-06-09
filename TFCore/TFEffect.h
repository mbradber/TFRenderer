#pragma once

#include <d3d11.h>
#include <string>
#include <vector>
#include "TFTypes.h"

namespace TFRendering
{
	class TFEffect
	{
	public:

		TFEffect();
		virtual ~TFEffect();

		void Initialize(ID3D11Device* a_pDevice,
			ID3D11DeviceContext* a_pDeviceContext,
			const std::wstring& a_wsShaderPath,
			const std::wstring& a_wsShaderPrefix);

		virtual void Init() = 0;

		void AddPixelShader(const std::wstring& a_sFilePathShader);
		void AddRenderable(class TFIRenderable* a_pRenderable);
		virtual void BatchDraw(const tfMatrix& a_matViewProj, 
			const tfMatrix& a_matLightVPT);

	private:
		// no copying
		TFEffect(TFEffect&);
		TFEffect& operator=(TFEffect&);

		void BuildVertexShader(const std::wstring& a_wsShaderPath);
		void QueryVertexShader();

	protected:
		ID3D11Device*			m_pDevice;
		ID3D11DeviceContext*	m_pDeviceContext;
		ID3D11InputLayout*		m_pInputLayout;
		ID3D11VertexShader*		m_pVertexShader;
		ID3D11PixelShader*		m_pPixelShader;
		std::wstring			m_wsShaderPrefix;
		UINT                    m_nStartSlotVB;
		UINT                    m_nNumVertexBuffers;
		ID3D11Buffer**          m_pVertexBuffers;
		std::vector<UINT>       m_vStrides;
		std::vector<UINT>       m_vOffsets;
		ID3D11Buffer*           m_pCBPerObject;


		std::vector<TFIRenderable*> m_vRenderables;
	};
}