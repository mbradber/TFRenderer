#pragma once

#include <d3d11.h>
#include <string>
#include <vector>
#include "TFModel.h"

namespace TFCore
{

	class TFTerrain
	{
	public:
		TFTerrain();
		~TFTerrain();

	void Init(ID3D11Device* a_pDevice, 
		ID3D11DeviceContext* a_pDeviceContext, 
		float a_fScale, 
		ID3D11VertexShader* a_pVertexShader,
		ID3D11PixelShader* a_pPixelShader,
		ID3D11InputLayout* a_pInputLayout,
		const std::string& a_sAssetPath);

	void Draw();
	void UpdateResources(const XMMATRIX& a_matWVP, 
		const XMMATRIX& a_matWorld, 
		const XMMATRIX& a_matLightWVPT, 
		const XMMATRIX& a_matTex, 
		const XMFLOAT3& a_vEyePos);

	void ActivateShaders();

	// TODO: CLEANUP COM CRAP IN DTOR
	private:
		void GenerateHeightMap(std::string a_sFilePath);
		void GenerateGrid(size_t a_nWidth, size_t a_nDepth);

		std::vector<unsigned char> m_hmData;
		ID3D11Device*			  m_pd3dDevice;
		ID3D11DeviceContext*	  m_pDeviceContext;
		ID3D11Buffer*			  m_pVertexBuffer;
		ID3D11Buffer*			  m_pIndexBuffer;
		size_t                    m_nIndexCount;
		TFMaterial                m_material;
		ID3D11Buffer*             m_pCBPerObject;
		ID3D11VertexShader*	      m_pVertexShader;
		ID3D11PixelShader*		  m_pPixelShader;
		ID3D11InputLayout*		  m_pInputLayout;
	};

}