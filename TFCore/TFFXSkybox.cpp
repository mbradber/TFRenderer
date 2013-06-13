#include "TFFXSkybox.h"
#include "TFUtils.h"
#include "TFIRenderable.h"

namespace TFRendering
{

	TFFXSkybox::TFFXSkybox(ID3D11Device* a_pDevice,
			ID3D11DeviceContext* a_pDeviceContext)
			:
			TFIEffect(a_pDevice, 
				a_pDeviceContext, 
				std::wstring(L"SkyVS.cso"),
				std::wstring(L"SkyPS.cso"))
	{
		// describe the cb for the per object data
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof(BufferPerObject);
		bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags      = 0;

		// Create the constant buffer with the device
		HR(m_pDevice->CreateBuffer(&bd, NULL, &m_pCBPerObject));

		// create shader resource view for cube map texture
		D3DX11CreateShaderResourceViewFromFile(m_pDevice, L"..\\Textures\\grasscube1024.dds", 0, 0, &m_cubeMapSRV, 0);
	}

	TFFXSkybox::~TFFXSkybox()
	{
		ReleaseCOM(m_cubeMapSRV);
	}

	void TFFXSkybox::BatchDraw(const tfMatrix& a_matViewProj, 
		const tfMatrix& a_matLightVPT)
	{
		SetRenderState();

		m_pDeviceContext->PSSetShaderResources(0, 1, &m_cubeMapSRV); // bind cube map SRV

		// render each renderable for this effect
		for(std::vector<TFIRenderable*>::iterator _itr = m_vRenderables.begin();
			_itr != m_vRenderables.end();
			++_itr)
		{
			TFIRenderable* _pRenderable = *_itr;

			// update per object constant buffer
			UpdateBuffers(_pRenderable->GetWorldMatrix(), a_matViewProj, a_matLightVPT);

			// set vertex/index buffers for this batch
			m_pVertexBuffers[0] = _pRenderable->GetPositionBuffer();

			m_pDeviceContext->IASetVertexBuffers(0, m_nNumVertexBuffers, m_pVertexBuffers, &m_vStrides[0], &m_vOffsets[0]);
			m_pDeviceContext->IASetIndexBuffer(_pRenderable->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

			_pRenderable->Draw();
		}
	}

	void TFFXSkybox::UpdateBuffers(const tfFloat4x4& a_matWorld, 
		const tfMatrix& a_matViewProj,
		const tfMatrix& a_matLightVPT)
	{
		//UPDATE TRANSFORM RESOURCE
		BufferPerObject cb;
		tfMatrix _matWorld = XMLoadFloat4x4(&a_matWorld);
		// update wvp of buffer
		cb.wvpMatrix = XMMatrixTranspose(_matWorld * a_matViewProj);

		m_pDeviceContext->UpdateSubresource(m_pCBPerObject, 0, NULL, &cb, 0, 0);
		m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCBPerObject);
	}

}