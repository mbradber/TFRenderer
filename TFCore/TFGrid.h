#pragma once

#include <d3d11.h>
#include <string>
#include <vector>
#include <fstream>
#include "TFModel.h"

namespace TFCore
{

	class TFGrid
	{
	public:
		TFGrid();
		virtual ~TFGrid();

		void Init(ID3D11Device* a_pDevice, 
			ID3D11DeviceContext* a_pDeviceContext, 
			ID3D11VertexShader* a_pVertexShader,
			ID3D11PixelShader* a_pPixelShader,
			ID3D11InputLayout* a_pInputLayout,
			const std::string& a_sHeightmap,
			int a_nGridWidth,
			int a_nGridDepth);

		void UpdateResources(const XMMATRIX& a_matWVP, 
			const XMMATRIX& a_matWorld, 
			const XMMATRIX& a_matLightWVPT, 
			const XMMATRIX& a_matTex, 
			const XMFLOAT3& a_vEyePos);

		void Draw();
		void ActivateShaders();

	protected:

		void GenerateGrid(int a_nWidth, int a_nDepth, float a_fTextureScale);
		void GenerateHeightMapFromRAW(std::string a_sFilePath, int a_nGridWidth, int a_nGridDepth);
		void GenerateHeightmapFromBMP(const std::string& a_sFilePath);
		int  GetNextValue4B(std::ifstream& a_bmStream);

	protected:

		std::vector<unsigned char> m_hmData;
		ID3D11Device*			   m_pd3dDevice;
		ID3D11DeviceContext*	   m_pDeviceContext;
		ID3D11Buffer*			   m_pVertexBuffer;
		ID3D11Buffer*			   m_pIndexBuffer;
		int                        m_nIndexCount;
		TFMaterial                 m_material;
		ID3D11Buffer*              m_pCBPerObject;
		ID3D11VertexShader*	       m_pVertexShader;
		ID3D11PixelShader*		   m_pPixelShader;
		ID3D11InputLayout*		   m_pInputLayout;
	};

}

