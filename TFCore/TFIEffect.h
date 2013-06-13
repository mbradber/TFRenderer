#pragma once

/*** Abstract interface representing an effect to be executed on the GPU. This serves as a batch rendering 
	 manager as it holds a container to renderable objects, grabs the vertex components
	 required from that renderable, then sets the state of the GPU to render the effect
	 (render states/shaders/buffer data/textures from renderables). ***/

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
		virtual void SetShadowMap(ID3D11ShaderResourceView* a_pShadowMap);
		virtual void UnbindShadowMap();
		virtual void SetReflectionMap(ID3D11ShaderResourceView* a_pReflectionMap);
		virtual void UnbindReflectionMap();
		virtual void SetBlendState();

		static void InitializeSamplers(ID3D11Device* a_pDevice,
			ID3D11DeviceContext* a_pDeviceContext);

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
		ID3D11Buffer*			m_pCBPerFrame;

		std::vector<TFIRenderable*> m_vRenderables;
	};
}