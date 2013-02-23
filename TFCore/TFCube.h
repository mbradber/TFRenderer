#pragma once
#include <d3d11.h>

namespace TFCore
{
	class TFCube
	{
	public:
		TFCube(void);
		TFCube(ID3D11Device* a_pDevice, ID3D11DeviceContext* a_pDeviceContext, float a_fScale);
		~TFCube(void);

		void Init(ID3D11Device* a_pDevice, ID3D11DeviceContext* a_pDeviceContext, float a_fScale);
		void Draw();

	private:
		ID3D11Device*        m_pd3dDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		ID3D11Buffer*        m_pVertexBuffer;
		ID3D11Buffer*        m_pIndexBuffer;
		float                m_fScale;
		const size_t         VERTEX_COUNT;
		const size_t         INDEX_COUNT;
	};

}



