#pragma once

#include <D3D11.h>

namespace TFRendering
{
	class TFShadowMap
	{
	public:
		TFShadowMap(ID3D11Device* a_pd3dDevice, ID3D11DeviceContext* a_pDeviceContext, size_t a_nWidth, size_t a_nHeight);
		~TFShadowMap();

		ID3D11ShaderResourceView* GetDepthMapSRV();
		void SetRenderState();

	private:
		TFShadowMap(const TFShadowMap&);
		TFShadowMap& operator=(const TFShadowMap&);

	private:
		size_t m_nWidth;
		size_t m_nHeight;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		ID3D11ShaderResourceView* m_pDepthMapSRV;
		ID3D11DepthStencilView* m_pDepthMapDSV;
		D3D11_VIEWPORT m_viewPort;
	};

}

