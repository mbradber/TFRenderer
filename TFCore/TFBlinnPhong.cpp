#include "TFBlinnPhong.h"
#include "TFModelEx.h"

namespace TFRendering
{

	TFBlinnPhong::TFBlinnPhong()
	{

	}


	TFBlinnPhong::~TFBlinnPhong()
	{

	}

	void TFBlinnPhong::BatchDraw()
	{
		// set state for this effect
		TFEffect::BatchDraw();


		for(std::vector<TFIRenderable*>::iterator _itr = m_vRenderables.begin();
			_itr != m_vRenderables.end();
			++_itr)
		{
			TFIRenderable* _pRenderable = *_itr;

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
}