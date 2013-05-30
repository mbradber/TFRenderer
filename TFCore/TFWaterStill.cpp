#include "TFWaterStill.h"

#include "TFUtils.h"

namespace TFCore
{

	TFWaterStill::TFWaterStill()
		:m_fWaterOffsetX(0),
		 m_fWaterOffsetY(0)
	{
		m_matTexTransform = XMMatrixIdentity();
	}


	TFWaterStill::~TFWaterStill()
	{

	}

	void TFWaterStill::Init(ID3D11Device* a_pDevice, 
		ID3D11DeviceContext* a_pDeviceContext, 
		ID3D11VertexShader* a_pVertexShader,
		ID3D11PixelShader* a_pPixelShader,
		ID3D11InputLayout* a_pInputLayout,
		const std::string& a_sHeightmap,
		int a_nGridWidth,
		int a_nGridDepth)
	{
		TFGrid::Init(a_pDevice, 
			a_pDeviceContext, 
			a_pVertexShader, 
			a_pPixelShader, 
			a_pInputLayout, 
			a_sHeightmap,
			a_nGridWidth, 
			a_nGridDepth);

		m_bUsingHeightmap = false;
		GenerateGrid(a_nGridWidth, a_nGridDepth, 12.0f);

		// VERTS

		//std::vector<TFPosNormTex4Tan> _vVertices(4);

		//// first vert of quad
		//_vVertices[0].Pos.x = -1.0f; // pos
		//_vVertices[0].Pos.y = 0.0f;
		//_vVertices[0].Pos.z = -1.0f;
		//
		//_vVertices[0].Norm.x = 0.0f; // normal
		//_vVertices[0].Norm.y = 1.0f;
		//_vVertices[0].Norm.z = 0.0f;

		//_vVertices[0].TanU.x = 1.0f; // tangents
		//_vVertices[0].TanU.y = 0.0f;
		//_vVertices[0].TanU.z = 0.0f;

		//_vVertices[0].TexC.x = 0.0f; // tex coords
		//_vVertices[0].TexC.y = 1.0f;

		//// second vert
		//_vVertices[1].Pos.x = 1.0f; // pos
		//_vVertices[1].Pos.y = 0.0f;
		//_vVertices[1].Pos.z = -1.0f;

		//_vVertices[1].Norm.x = 0.0f; // normal
		//_vVertices[1].Norm.y = 1.0f;
		//_vVertices[1].Norm.z = 0.0f;

		//_vVertices[1].TanU.x = 1.0f; // tangents
		//_vVertices[1].TanU.y = 0.0f;
		//_vVertices[1].TanU.z = 0.0f;

		//_vVertices[1].TexC.x = 1.0f; // tex coords
		//_vVertices[1].TexC.y = 1.0f;

		//// third vert
		//_vVertices[2].Pos.x = 1.0f;
		//_vVertices[2].Pos.y = 0.0f;
		//_vVertices[2].Pos.z = 1.0f;

		//_vVertices[2].Norm.x = 0.0f; // normal
		//_vVertices[2].Norm.y = 1.0f;
		//_vVertices[2].Norm.z = 0.0f;

		//_vVertices[2].TanU.x = 1.0f; // tangents
		//_vVertices[2].TanU.y = 0.0f;
		//_vVertices[2].TanU.z = 0.0f;

		//_vVertices[2].TexC.x = 1.0f; // tex coords
		//_vVertices[2].TexC.y = 0.0f;

		////fourth vert
		//_vVertices[3].Pos.x = -1.0f;
		//_vVertices[3].Pos.y = 0.0f;
		//_vVertices[3].Pos.z = 1.0f;

		//_vVertices[3].Norm.x = 0.0f; // normal
		//_vVertices[3].Norm.y = 1.0f;
		//_vVertices[3].Norm.z = 0.0f;

		//_vVertices[3].TanU.x = 1.0f; // tangents
		//_vVertices[3].TanU.y = 0.0f;
		//_vVertices[3].TanU.z = 0.0f;

		//_vVertices[3].TexC.x = 0.0f; // tex coords
		//_vVertices[3].TexC.y = 0.0f;

		//// INDICES

		//std::vector<int> _vIndices;

		//_vIndices.push_back(3);
		//_vIndices.push_back(2);
		//_vIndices.push_back(0);
		//_vIndices.push_back(0);
		//_vIndices.push_back(2);
		//_vIndices.push_back(1);

		//m_nIndexCount = 6;

		//// BUFFERS

		//// describe this buffer
		//D3D11_BUFFER_DESC bd;
		//ZeroMemory( &bd, sizeof(bd) );
		//bd.Usage          = D3D11_USAGE_DEFAULT;
		//bd.ByteWidth      = sizeof( TFPosNormTex4Tan ) * _vVertices.size();
		//bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
		//bd.CPUAccessFlags = 0; // No cpu access
		//bd.MiscFlags      = 0; // Unused

		//D3D11_SUBRESOURCE_DATA InitData;
		//ZeroMemory( &InitData, sizeof(InitData) );
		//InitData.pSysMem = &_vVertices[0];

		//// Create a buffer to hold this cube's vert data in video memory
		//HR(m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer));

		//D3D11_BUFFER_DESC ibd;
		//ZeroMemory(&ibd, sizeof(ibd));
		//ibd.Usage = D3D11_USAGE_IMMUTABLE;
		//ibd.ByteWidth = sizeof(int) * _vIndices.size();
		//ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		//ibd.CPUAccessFlags = 0;
		//ibd.MiscFlags = 0;
		//D3D11_SUBRESOURCE_DATA iinitData;
		//iinitData.pSysMem = &_vIndices[0];

		//HR(m_pd3dDevice->CreateBuffer(&ibd, &iinitData, &m_pIndexBuffer));

		//// GENERATE SHADER RESOURCES

		//// describe the cb for the WVP matrix
		//D3D11_BUFFER_DESC sbd;
		//ZeroMemory(&sbd, sizeof(sbd));
		//sbd.Usage          = D3D11_USAGE_DEFAULT;
		//sbd.ByteWidth      = sizeof(TFBufferPerObject);
		//sbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		//sbd.CPUAccessFlags = 0;
		//sbd.MiscFlags      = 0;

		//// Create the constant buffer with the device
		//HR(m_pd3dDevice->CreateBuffer(&sbd, NULL, &m_pCBPerObject));

		// Bind normal map
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pd3dDevice, 
			L"..\\Textures\\water_normal.png", 
			NULL, 
			NULL, 
			&m_pNormalMapSRV, 
			NULL));
	}

	void TFWaterStill::Update(float a_fDelta)
	{
		m_fWaterOffsetX += 0.25f * a_fDelta;
		//m_fWaterOffsetY += 0.1f * a_fDelta;
		//m_fWaterOffsetX = 0.25f * sinf(12.0f * m_fWaterOffsetY);

		m_matTexTransform = XMMatrixTranslation(m_fWaterOffsetX, m_fWaterOffsetY, 0);


	}

	XMMATRIX TFWaterStill::GetTextureTransform() const
	{
		return m_matTexTransform;
	}

	void TFWaterStill::BindReflectionMap(ID3D11ShaderResourceView* a_pReflectionMap)
	{
		m_pDeviceContext->PSSetShaderResources(0, 1, &a_pReflectionMap);
	}

	void TFWaterStill::ActivateShaders()
	{
		TFGrid::ActivateShaders();

		// bind normal map for water
		m_pDeviceContext->PSSetShaderResources(1, 1, &m_pNormalMapSRV);
	}

}
