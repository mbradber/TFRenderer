#pragma once

#include <d3d11.h>
#include <string>
#include "TFTypes.h"

namespace TFRendering
{
	class TFIRenderable
	{
	public:

		TFIRenderable();
		virtual ~TFIRenderable() {};

		virtual void SetWorldMatrix(const tfMatrix& a_matWorld);
		virtual tfFloat4x4& GetWorldMatrix();

		virtual void Draw() = 0;

		// buffer access
		ID3D11Buffer* GetPositionBuffer() const { return m_pPositionVertexBuffer; }
		ID3D11Buffer* GetNormalBuffer()   const { return m_pNormalVertexBuffer;   }
		ID3D11Buffer* GetTexCoordBuffer() const { return m_pTexCoordVertexBuffer; }
		ID3D11Buffer* GetTangentBuffer()  const { return m_pTangentVertexBuffer;  }
		ID3D11Buffer* GetIndexBuffer()    const { return m_pIndexBuffer;          }

	protected:
		ID3D11Device*			m_pDevice;
		ID3D11DeviceContext*	m_pDeviceContext;
		tfFloat4x4              m_matWorld;

		// buffers
		ID3D11Buffer* m_pPositionVertexBuffer;
		ID3D11Buffer* m_pNormalVertexBuffer;
		ID3D11Buffer* m_pTexCoordVertexBuffer;
		ID3D11Buffer* m_pTangentVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
	};
}