#include "TFTerrain.h"
#include <fstream>
#include <vector>

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
		float a_fScale, 
		ID3D11VertexShader* a_pVertexShader,
		ID3D11PixelShader* a_pPixelShader,
		ID3D11InputLayout* a_pInputLayout,
		const std::string& a_sAssetPath)
	{
		m_pd3dDevice = a_pDevice;
		m_pDeviceContext = a_pDeviceContext;

		GenerateHeightMap(a_sAssetPath);
	}

	void TFTerrain::GenerateHeightMap(std::string a_sFilePath)
	{
		//std::vector<unsigned char> m_hmData(2049 * 2049);
		m_hmData.resize(2049 * 2049);

		ifstream _terrainStream(a_sFilePath, ifstream::binary);

		if(_terrainStream.is_open())
		{
			_terrainStream.read((char*)&m_hmData[0], (std::streamsize)m_hmData.size());
			_terrainStream.close();
		}
		else
		{
			
		}

	}

}