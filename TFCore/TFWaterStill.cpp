#include "TFWaterStill.h"


namespace TFCore
{

	TFWaterStill::TFWaterStill()
	{

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
		GenerateGrid(a_nGridWidth, a_nGridDepth, 16.0f);
	}


}
