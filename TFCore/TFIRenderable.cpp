#include "TFIRenderable.h"


namespace TFRendering
{
	TFIRenderable::TFIRenderable()
		:m_pDevice(NULL),
		 m_pDeviceContext(NULL),
		 m_pPositionVertexBuffer(NULL),
		 m_pNormalVertexBuffer(NULL),
		 m_pTexCoordVertexBuffer(NULL),
		 m_pTangentVertexBuffer(NULL)
	{

	}

	void TFIRenderable::SetWorldMatrix(const tfMatrix& a_matWorld)
	{
		XMStoreFloat4x4(&m_matWorld, a_matWorld);
	}

	tfFloat4x4& TFIRenderable::GetWorldMatrix()
	{
		return m_matWorld;
	}
}