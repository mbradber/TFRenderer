#include "TFShadowMap.h"
#include "TFUtils.h"

namespace TFRendering
{
	TFShadowMap::TFShadowMap(ID3D11Device* a_pd3dDevice, ID3D11DeviceContext* a_pDeviceContext, size_t a_nWidth, size_t a_nHeight)
		: m_pDevice(a_pd3dDevice),
		  m_pDeviceContext(a_pDeviceContext),
		  m_nWidth(a_nWidth),
		  m_nHeight(a_nHeight),
		  m_pDepthMapSRV(NULL),
		  m_pDepthMapDSV(NULL)
	{
		m_viewPort.TopLeftX = 0.0f;
		m_viewPort.TopLeftY = 0.0f;
		m_viewPort.Width = static_cast<float>(m_nWidth);
		m_viewPort.Height = static_cast<float>(m_nHeight);
		m_viewPort.MinDepth = 0.0f;
		m_viewPort.MaxDepth = 1.0f;

		// Use typeless format because the DSV is going to interpret
		// the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going to interpret
		// the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width     = m_nWidth;
		texDesc.Height    = m_nHeight;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format    = DXGI_FORMAT_R24G8_TYPELESS;
		texDesc.SampleDesc.Count   = 1;  
		texDesc.SampleDesc.Quality = 0;  
		texDesc.Usage          = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0; 
		texDesc.MiscFlags      = 0;

		ID3D11Texture2D* _pDepthMap = 0;
		HR(m_pDevice->CreateTexture2D(&texDesc, 0, &_pDepthMap));

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = 0;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		HR(m_pDevice->CreateDepthStencilView(_pDepthMap, &dsvDesc, &m_pDepthMapDSV));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		HR(m_pDevice->CreateShaderResourceView(_pDepthMap, &srvDesc, &m_pDepthMapSRV));

		// View saves a reference to the texture so we can release our reference.
		ReleaseCOM(_pDepthMap);
	}

	TFShadowMap::~TFShadowMap()
	{

	}

	ID3D11ShaderResourceView* TFShadowMap::GetDepthMapSRV()
	{
		return m_pDepthMapSRV;
	}

	void TFShadowMap::SetRenderState()
	{
		TF_ASSERT(m_pDeviceContext != NULL, FILE_NAME, LINE_NO);

		m_pDeviceContext->RSSetViewports(1, &m_viewPort);

		ID3D11RenderTargetView* _renderTargets[1] = {0};
		m_pDeviceContext->OMSetRenderTargets(1, _renderTargets, m_pDepthMapDSV);

		m_pDeviceContext->ClearDepthStencilView(m_pDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

}
