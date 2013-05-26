#pragma once

#include "TFGrid.h"

namespace TFCore
{

	class TFWaterStill : public TFGrid
	{
	public:
		TFWaterStill();
		~TFWaterStill();

		void TFWaterStill::Init(ID3D11Device* a_pDevice, 
			ID3D11DeviceContext* a_pDeviceContext, 
			ID3D11VertexShader* a_pVertexShader,
			ID3D11PixelShader* a_pPixelShader,
			ID3D11InputLayout* a_pInputLayout,
			const std::string& a_sHeightmap,
			int a_nGridWidth,
			int a_nGridDepth);

		void BindReflectionMap(ID3D11ShaderResourceView*);
	};

}

