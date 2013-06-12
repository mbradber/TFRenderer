#include "TFFXTerrain.h"
#include "TFUtils.h"
#include "TFIRenderable.h"

namespace TFRendering
{

	using namespace DirectX;

	TFFXTerrain::TFFXTerrain(ID3D11Device* a_pDevice,
		ID3D11DeviceContext* a_pDeviceContext)
		:TFIEffect(a_pDevice, 
			a_pDeviceContext, 
			std::wstring(L"TerrainVS.cso"),
			std::wstring(L"TerrainPS.cso"))
		
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
	}


	TFFXTerrain::~TFFXTerrain()
	{

	}

	void TFFXTerrain::BatchDraw(const tfMatrix& a_matViewProj, 
		const tfMatrix& a_matLightVPT)
	{
		SetRenderState();

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
			m_pVertexBuffers[1] = _pRenderable->GetNormalBuffer();
			m_pVertexBuffers[2] = _pRenderable->GetTexCoordBuffer();

			m_pDeviceContext->IASetVertexBuffers(0, m_nNumVertexBuffers, m_pVertexBuffers, &m_vStrides[0], &m_vOffsets[0]);
			m_pDeviceContext->IASetIndexBuffer(_pRenderable->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

			_pRenderable->Draw();
		}
	}

	void TFFXTerrain::UpdateBuffers(const tfFloat4x4& a_matWorld, 
		const tfMatrix& a_matViewProj,
		const tfMatrix& a_matLightVPT)
	{
		//UPDATE TRANSFORM RESOURCE
		BufferPerObject cb;

		tfMatrix _matWorld = XMLoadFloat4x4(&a_matWorld);

		// update world matrix
		cb.worldMatrix = XMMatrixTranspose(_matWorld);

		// Update world inverse transpose matrix (used to transform normals as it will be distorted 
		// with non uniform scaling transforms, see pg. 277 of Luna...
		tfMatrix _wit = _matWorld;
		_wit.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		tfVector _witDet = XMMatrixDeterminant(_wit);

		cb.worldInvTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(&_witDet, _wit));

		// update wvp of buffer
		cb.wvpMatrix = XMMatrixTranspose(_matWorld * a_matViewProj);

		// update matrix to transform from object to projective texture coords
		cb.lightVPT = XMMatrixTranspose(_matWorld * a_matLightVPT);

		m_pDeviceContext->UpdateSubresource(m_pCBPerObject, 0, NULL, &cb, 0, 0);

		m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCBPerObject);
	}

	void TFFXTerrain::SetShadowMap(ID3D11ShaderResourceView* a_pShadowMap)
	{
		m_pDeviceContext->PSSetShaderResources(6, 1, &a_pShadowMap);
	}

	void TFFXTerrain::UnbindShadowMap()
	{
		ID3D11ShaderResourceView* _pSRV[1] = {NULL};
		m_pDeviceContext->PSSetShaderResources(6, 1, _pSRV);
	}

}