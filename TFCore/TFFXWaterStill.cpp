#include "TFFXWaterStill.h"
#include "TFUtils.h"
#include "TFIRenderable.h"

namespace TFRendering
{

	using namespace DirectX;

	TFFXWaterStill::TFFXWaterStill(ID3D11Device* a_pDevice,
		ID3D11DeviceContext* a_pDeviceContext)
		:TFIEffect(a_pDevice,
			a_pDeviceContext,
			std::wstring(L"StillWaterVS.cso"),
			std::wstring(L"StillWaterPS.cso")),
			m_fWaterOffsetX(0.0f),
			m_fWaterOffsetY(0.0f)
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

		// create blend state for water
		D3D11_BLEND_DESC blendDesc1 = {0};
		blendDesc1.AlphaToCoverageEnable = false;
		blendDesc1.IndependentBlendEnable = false;

		blendDesc1.RenderTarget[0].BlendEnable = true;
		blendDesc1.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc1.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc1.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc1.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc1.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc1.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc1.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		m_pDevice->CreateBlendState(&blendDesc1, &m_pBlendState1);
	}


	TFFXWaterStill::~TFFXWaterStill()
	{
		ReleaseCOM(m_pBlendState1);
	}

	void TFFXWaterStill::BatchDraw(const tfMatrix& a_matViewProj, 
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
			m_pVertexBuffers[3] = _pRenderable->GetTangentBuffer();

			m_pDeviceContext->IASetVertexBuffers(0, m_nNumVertexBuffers, m_pVertexBuffers, &m_vStrides[0], &m_vOffsets[0]);
			m_pDeviceContext->IASetIndexBuffer(_pRenderable->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

			_pRenderable->Draw();
		}
	}

	void TFFXWaterStill::UpdateBuffers(const tfFloat4x4& a_matWorld, 
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



		// update texture transform matrices
		XMMATRIX _matTexTransformPos = XMMatrixTranslation(fmod(m_fWaterOffsetX, 12.0f), 0, 0);
		XMMATRIX _matTexTransformNeg = XMMatrixTranslation(fmod(m_fWaterOffsetY, 12.0f), 0, 0)
				* XMMatrixScaling(0.5f, 0.5f, 0.5f);

		// update matrix to transform from object to projective texture coords
		cb.texTransformNeg = XMMatrixTranspose(_matTexTransformNeg);
		cb.texTransformPos = XMMatrixTranspose(_matTexTransformPos);

		m_pDeviceContext->UpdateSubresource(m_pCBPerObject, 0, NULL, &cb, 0, 0);
		m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCBPerObject);
	}

	void TFFXWaterStill::SetReflectionMap(ID3D11ShaderResourceView* a_pReflectionMap)
	{
		m_pDeviceContext->PSSetShaderResources(0, 1, &a_pReflectionMap);
	}

	void TFFXWaterStill::UnbindReflectionMap()
	{
		ID3D11ShaderResourceView* _pSRV[1] = {NULL};
		m_pDeviceContext->PSSetShaderResources(0, 1, _pSRV);
	}

	void TFFXWaterStill::UpdateWater(float a_fDelta)
	{
		m_fWaterOffsetX += a_fDelta * 0.1f;
		m_fWaterOffsetY += -a_fDelta * 0.1f;
	}

	void TFFXWaterStill::SetBlendState()
	{
		float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_pDeviceContext->OMSetBlendState(m_pBlendState1, blendFactors, 0xffffffff);
	}

}