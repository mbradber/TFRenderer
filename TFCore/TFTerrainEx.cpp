#include "TFTerrainEx.h"
#include "TFUtils.h"

namespace TFRendering
{

	TFTerrainEx::TFTerrainEx(ID3D11Device* a_pDevice,
		ID3D11DeviceContext* a_pDeviceContext,
		const std::string& a_sHeightMap,
		int a_nGridWidth,
		int a_nGridDepth)
		:
		TFGridEx(a_pDevice,
			a_pDeviceContext,
			a_sHeightMap,
			a_nGridWidth,
			a_nGridDepth)
	{
		GenerateHeightmapFromBMP(a_sHeightMap);
		GenerateGrid(a_nGridWidth, a_nGridDepth, 16.0f);

		// grass
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pDevice, 
			L"..\\Textures\\Terrain\\grass.dds", 
			NULL, 
			NULL, 
			&m_pTexture1SRV, 
			NULL));

		// light dirt
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pDevice, 
			L"..\\Textures\\Terrain\\lightdirt.dds", 
			NULL, 
			NULL, 
			&m_pTexture2SRV, 
			NULL));

		// dark dirt
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pDevice, 
			L"..\\Textures\\Terrain\\grass_dull.dds", 
			NULL, 
			NULL, 
			&m_pTexture3SRV, 
			NULL));

		// stone
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pDevice, 
			L"..\\Textures\\Terrain\\darkdirt.dds", 
			NULL, 
			NULL, 
			&m_pTexture4SRV, 
			NULL));

		// load the blend map for the terrain grass
		HR(D3DX11CreateShaderResourceViewFromFile(m_pDevice, 
			L"..\\Textures\\Terrain\\blend_map_grass.png",  
			NULL, 
			NULL, 
			&m_pBlendMapGrassSRV, 
			NULL));

		// load the blend map for the terrain rock
		HR(D3DX11CreateShaderResourceViewFromFile(m_pDevice, 
			L"..\\Textures\\Terrain\\blend_map_dirt.png", 
			NULL, 
			NULL, 
			&m_pBlendMapDirtSRV, 
			NULL));
	}


	TFTerrainEx::~TFTerrainEx()
	{
		ReleaseCOM(m_pTexture1SRV);
		ReleaseCOM(m_pTexture2SRV);
		ReleaseCOM(m_pTexture3SRV);
		ReleaseCOM(m_pTexture4SRV);
		ReleaseCOM(m_pBlendMapGrassSRV);
		ReleaseCOM(m_pBlendMapDirtSRV);
	}

	void TFTerrainEx::Draw()
	{
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