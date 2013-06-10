#pragma once

#include "TFIRenderable.h"
#include <vector>
#include <fstream>

namespace TFRendering
{

	class TFGridEx : public TFIRenderable
	{
	public:
		TFGridEx(ID3D11Device* a_pDevice, 
			ID3D11DeviceContext* a_pDeviceContext,
			const std::string& a_sHeightmap,
			int a_nGridWidth,
			int a_nGridDepth);
		virtual ~TFGridEx();

	protected:
		void GenerateHeightmapFromBMP(const std::string& a_sFilePath);
		int  GetNextValue4B(std::ifstream& a_bmStream);
		void GenerateGrid(int a_nWidth, int a_nDepth, float a_fTextureScale);

	private:
		std::vector<unsigned char> m_hmData;
		bool                       m_bUsingHeightmap;
	};
}