#pragma once
#include "TFGridEx.h"

namespace TFRendering
{

	class TFTerrainEx : public TFGridEx
	{
	public:
		TFTerrainEx(ID3D11Device* a_pDevice,
			ID3D11DeviceContext* a_pDeviceContext,
			const std::string& a_sHeightMap,
			int a_nGridWidth,
			int a_nGridDepth);
		~TFTerrainEx();

		void Draw();

	private:
		// disable copying
		TFTerrainEx(TFTerrainEx&);
		TFTerrainEx& operator=(TFTerrainEx&);

		ID3D11ShaderResourceView* m_pTexture1SRV;
		ID3D11ShaderResourceView* m_pTexture2SRV;
		ID3D11ShaderResourceView* m_pTexture3SRV;
		ID3D11ShaderResourceView* m_pTexture4SRV;
		ID3D11ShaderResourceView* m_pBlendMapGrassSRV;
		ID3D11ShaderResourceView* m_pBlendMapDirtSRV;
	};

}