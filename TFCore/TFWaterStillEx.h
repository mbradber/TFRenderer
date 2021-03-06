#pragma once
#include "TFGridEx.h"

namespace TFRendering
{

	class TFWaterStillEx : public TFGridEx
	{
	public:
		TFWaterStillEx(ID3D11Device* a_pDevice,
			ID3D11DeviceContext* a_pDeviceContext,
			int a_nGridWidth,
			int a_nGridDepth);
		~TFWaterStillEx();

		void Draw();

	private:
		ID3D11ShaderResourceView* m_pNormalMapSRV;
	};


}