#include "TFIRenderable.h"


namespace TFRendering
{
	TFIRenderable::TFIRenderable(ID3D11Device* a_pDevice, 
		ID3D11DeviceContext* a_pDeviceContext)
		:m_pDevice(a_pDevice),
		 m_pDeviceContext(a_pDeviceContext),
		 m_nVertexCount(0),
		 m_nIndexCount(0),
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