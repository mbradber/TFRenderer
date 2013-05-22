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
		ID3D11VertexShader* a_pVertexShader,
		ID3D11PixelShader* a_pPixelShader,
		ID3D11InputLayout* a_pInputLayout,
		const std::string& a_sAssetPath,
		const std::wstring& a_sBlendMap,
		int a_nGridSize);

	void Draw();
	void UpdateResources(const XMMATRIX& a_matWVP, 
		const XMMATRIX& a_matWorld, 
		const XMMATRIX& a_matLightWVPT, 
		const XMMATRIX& a_matTex, 
		const XMFLOAT3& a_vEyePos);

	void ActivateShaders();

	void AddTexture(const std::string& a_sTexturePath);

	// TODO: CLEANUP COM CRAP IN DTOR
	private:
		void GenerateHeightMap(std::string a_sFilePath, int a_nGridSize);
		void GenerateGrid(int a_nWidth, int a_nDepth, float a_fTextureScale);

		std::vector<unsigned char> m_hmData;
		std::wstring              m_wsBlendMapPath;
		ID3D11Device*			  m_pd3dDevice;
		ID3D11DeviceContext*	  m_pDeviceContext;
		ID3D11Buffer*			  m_pVertexBuffer;
		ID3D11Buffer*			  m_pIndexBuffer;
		int                       m_nIndexCount;
		TFMaterial                m_material;
		ID3D11Buffer*             m_pCBPerObject;
		ID3D11VertexShader*	      m_pVertexShader;
		ID3D11PixelShader*		  m_pPixelShader;
		ID3D11InputLayout*		  m_pInputLayout;
		ID3D11ShaderResourceView* m_pTexture1SRV;
		ID3D11ShaderResourceView* m_pTexture2SRV;
		ID3D11ShaderResourceView* m_pTexture3SRV;
		ID3D11ShaderResourceView* m_pTexture4SRV;
		ID3D11ShaderResourceView* m_pBlendMapGrassSRV;
		ID3D11ShaderResourceView* m_pBlendMapDirtSRV;
	};

}