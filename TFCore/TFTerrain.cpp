#include "TFTerrain.h"
#include <fstream>
#include <vector>
#include "TFVertices.h"
#include "TFUtils.h"

using namespace std;

namespace TFCore
{

	TFTerrain::TFTerrain()
	{

	}

	TFTerrain::~TFTerrain()
	{
	}

	void TFTerrain::Init(ID3D11Device* a_pDevice, 
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

		GenerateHeightmapFromBMP(a_sHeightmap);
		GenerateGrid(a_nGridWidth, a_nGridDepth, 16.0f);

		// GENERATE SHADER RESOURCES

		// describe the cb for the WVP matrix
		D3D11_BUFFER_DESC sbd;
		ZeroMemory(&sbd, sizeof(sbd));
		sbd.Usage          = D3D11_USAGE_DEFAULT;
		sbd.ByteWidth      = sizeof(XMFLOAT4); // must be multiple of 16 for constant buffer
		sbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		sbd.CPUAccessFlags = 0;
		sbd.MiscFlags      = 0;

		// Create the constant buffer with the device
		HR(m_pd3dDevice->CreateBuffer(&sbd, NULL, &m_pCBPerFrame));
	}

	void TFTerrain::GenerateGrid(int a_nWidth, int a_nDepth, float a_fTextureScale)
	{
		// Generate grid geometry
		TFGrid::GenerateGrid(a_nWidth, a_nDepth, a_fTextureScale);

		// add errain textures

		// grass
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pd3dDevice, 
			L"..\\Textures\\Terrain\\grass.dds", 
			NULL, 
			NULL, 
			&m_pTexture1SRV, 
			NULL));

		// light dirt
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pd3dDevice, 
			L"..\\Textures\\Terrain\\lightdirt.dds", 
			NULL, 
			NULL, 
			&m_pTexture2SRV, 
			NULL));

		// dark dirt
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pd3dDevice, 
			L"..\\Textures\\Terrain\\grass_dull.dds", 
			NULL, 
			NULL, 
			&m_pTexture3SRV, 
			NULL));

		// stone
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pd3dDevice, 
			L"..\\Textures\\Terrain\\darkdirt.dds", 
			NULL, 
			NULL, 
			&m_pTexture4SRV, 
			NULL));

		// load the blend map for the terrain grass
		HR(D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, 
			L"..\\Textures\\Terrain\\blend_map_grass.png",  
			NULL, 
			NULL, 
			&m_pBlendMapGrassSRV, 
			NULL));

		// load the blend map for the terrain rock
		HR(D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, 
			L"..\\Textures\\Terrain\\blend_map_dirt.png", 
			NULL, 
			NULL, 
			&m_pBlendMapDirtSRV, 
			NULL));
	}

	void TFTerrain::UpdateFrameData(XMFLOAT4 a_f4Data)
	{
		// update resource
		m_pDeviceContext->UpdateSubresource(m_pCBPerFrame , 0, NULL, &a_f4Data, 0, 0);

		// bind to vertex shader for terrain
		m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pCBPerFrame);
	}

	// TODO: Its inefficient to be setting all these states per draw call, should 
	// only do it once per batch of renderable types
	void TFTerrain::Draw()
	{
		// Set vertex buffers
		UINT _nStride = sizeof(TFPosNormTex4Tan);
		UINT _nOffset = 0;

		// bind vertex buffer to input assembler
		m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &_nStride, &_nOffset);

		// bind index buffer to input assembler
		m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		// Set primitive topology
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Bind terrain textures
		m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture1SRV);
		m_pDeviceContext->PSSetShaderResources(1, 1, &m_pTexture2SRV);
		m_pDeviceContext->PSSetShaderResources(2, 1, &m_pTexture3SRV);
		m_pDeviceContext->PSSetShaderResources(3, 1, &m_pTexture4SRV);

		// bind blend map textures
		m_pDeviceContext->PSSetShaderResources(4, 1, &m_pBlendMapGrassSRV);
		m_pDeviceContext->PSSetShaderResources(5, 1, &m_pBlendMapDirtSRV);

		// Draw self
		m_pDeviceContext->DrawIndexed(m_nIndexCount, 0, 0);
	}

}