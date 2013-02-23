#include "TFCube.h"
#include "TFVertices.h"
#include "TFUtils.h"

namespace TFCore
{

	TFCube::TFCube()
		:m_pd3dDevice(NULL),
	     m_pDeviceContext(NULL),
		 m_pVertexBuffer(NULL),
		 m_pIndexBuffer(NULL),
		 m_fScale(1.0f),
		 VERTEX_COUNT(8),
		 INDEX_COUNT(36)
	{
	}

	TFCube::TFCube(ID3D11Device* a_pDevice, ID3D11DeviceContext* a_pDeviceContext, float a_fScale)
		:m_pd3dDevice(a_pDevice),
		 m_pDeviceContext(a_pDeviceContext),
		 m_pVertexBuffer(NULL),
		 m_pIndexBuffer(NULL),
		 m_fScale(a_fScale),
		 VERTEX_COUNT(8),
		 INDEX_COUNT(36)
	{
	}


	TFCube::~TFCube()
	{
		ReleaseCOM(m_pVertexBuffer);
	}

	void TFCube::Init(ID3D11Device* a_pDevice, ID3D11DeviceContext* a_pDeviceContext, float a_fScale)
	{
		m_pd3dDevice     = a_pDevice;
		m_pDeviceContext = a_pDeviceContext;
		m_fScale         = a_fScale;

		// static vertex data for cube geometry
		TFSimpleVertex vertices[] = 
		{
			{ tfFloat3( -1.0f, 1.0f, -1.0f ), tfFloat4( 0.0f, 0.0f, 1.0f, 1.0f ) },
			{ tfFloat3( 1.0f, 1.0f, -1.0f ),  tfFloat4( 0.0f, 1.0f, 0.0f, 1.0f ) },
			{ tfFloat3( 1.0f, 1.0f, 1.0f ),   tfFloat4( 0.0f, 1.0f, 1.0f, 1.0f ) },
			{ tfFloat3( -1.0f, 1.0f, 1.0f ),  tfFloat4( 1.0f, 0.0f, 0.0f, 1.0f ) },
			{ tfFloat3( -1.0f, -1.0f, -1.0f ),tfFloat4( 1.0f, 0.0f, 1.0f, 1.0f ) },
			{ tfFloat3( 1.0f, -1.0f, -1.0f ), tfFloat4( 1.0f, 1.0f, 0.0f, 1.0f ) },
			{ tfFloat3( 1.0f, -1.0f, 1.0f ),  tfFloat4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ tfFloat3( -1.0f, -1.0f, 1.0f ), tfFloat4( 0.0f, 0.0f, 0.0f, 1.0f ) },
		};

		// scale the geometry
		for(size_t i = 0; i < VERTEX_COUNT; ++i)
		{
			vertices[i].Pos.x *= a_fScale;
			vertices[i].Pos.y *= a_fScale;
			vertices[i].Pos.z *= a_fScale;
		}

		// describe this buffer
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof( TFSimpleVertex ) * VERTEX_COUNT;
		bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0; // No cpu access
		bd.MiscFlags      = 0; // Unused

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory( &InitData, sizeof(InitData) );
		InitData.pSysMem = vertices;

		// Create a buffer to hold this cube's vert data in video memory
		HR(m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer));

		// Create index buffer
		WORD indices[] =
		{
			3,1,0,
			2,1,3,

			0,5,4,
			1,5,0,

			3,4,7,
			0,4,3,

			1,6,5,
			2,6,1,

			2,7,6,
			3,7,2,

			6,4,5,
			7,4,6,
		};

		// describe this index buffer
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof( WORD ) * INDEX_COUNT;
		bd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags      = 0; // Unused
		InitData.pSysMem  = indices;
		HR(m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer));
	}

	void TFCube::Draw()
	{
		// Set vertex buffer
		size_t stride = sizeof( TFSimpleVertex );
		size_t offset = 0;
		m_pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );
		// Set index buffer
		m_pDeviceContext->IASetIndexBuffer( m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
		// Set primitive topology
		m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		// Draw self
		m_pDeviceContext->DrawIndexed(INDEX_COUNT, 0, 0);
	}
}
