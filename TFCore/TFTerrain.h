#pragma once

#include <string>
#include <vector>
#include <d3d11.h>

namespace TFCore
{

	class TFTerrain
	{
	public:
		TFTerrain();
		~TFTerrain();

	void TFTerrain::Init(ID3D11Device* a_pDevice, 
		ID3D11DeviceContext* a_pDeviceContext, 
		float a_fScale, 
		ID3D11VertexShader* a_pVertexShader,
		ID3D11PixelShader* a_pPixelShader,
		ID3D11InputLayout* a_pInputLayout,
		const std::string& a_sAssetPath);

	private:
		void GenerateHeightMap(std::string a_sFilePath);

		std::vector<unsigned char> m_hmData;
		ID3D11Device*			  m_pd3dDevice;
		ID3D11DeviceContext*	  m_pDeviceContext;
		ID3D11Buffer*			  m_pVertexBuffer;
		ID3D11Buffer*			  m_pIndexBuffer;
	};

}