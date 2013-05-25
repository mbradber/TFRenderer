#pragma once

#include <d3d11.h>

namespace TFRendering
{

	class TFReflectionMap
	{
	public:
		TFReflectionMap(ID3D11Device* a_pd3dDevice, 
			ID3D11DeviceContext* a_pDeviceContext, 
			size_t a_nWidth, 
			size_t a_nHeight);

		~TFReflectionMap();

		void SetRenderTarget();
		ID3D11ShaderResourceView* GetReflectionMapSRV();

	private:
		TFReflectionMap(const TFReflectionMap&);
		TFReflectionMap& operator=(const TFReflectionMap&);

	private:
		size_t m_nWidth;
		size_t m_nHeight;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		ID3D11RenderTargetView* m_pRTV;
		ID3D11ShaderResourceView* m_pReflectionSRV;
		ID3D11DepthStencilView* m_pDSV;
		D3D11_VIEWPORT m_viewPort;
	};

}

