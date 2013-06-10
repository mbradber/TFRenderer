#include "TFWaterStillEx.h"
#include "TFUtils.h"

namespace TFRendering
{

	TFWaterStillEx::TFWaterStillEx(ID3D11Device* a_pDevice,
		ID3D11DeviceContext* a_pDeviceContext,
		int a_nGridWidth,
		int a_nGridDepth)
		:
		TFGridEx(a_pDevice,
			a_pDeviceContext,
			"", // not using heightmap
			a_nGridWidth,
			a_nGridDepth),
		m_pNormalMapSRV(NULL),
		m_fWaterOffsetX(0.0f),
		m_fWaterOffsetY(0.0f)
	{
		GenerateGrid(a_nGridWidth, a_nGridDepth, 20.0f);

		// Bind normal map
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pDevice, 
			L"..\\Textures\\water_normal.png", 
			NULL, 
			NULL, 
			&m_pNormalMapSRV, 
			NULL));
	}


	TFWaterStillEx::~TFWaterStillEx()
	{
		ReleaseCOM(m_pNormalMapSRV);
	}

	void TFWaterStillEx::Draw()
	{
		// bind normal map for water
		m_pDeviceContext->PSSetShaderResources(1, 1, &m_pNormalMapSRV);

		// Draw self
		m_pDeviceContext->DrawIndexed(m_nIndexCount, 0, 0);
	}


}