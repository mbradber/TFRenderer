#pragma once

#include "TFGrid.h"

namespace TFCore
{

	// Layout of the buffer for vertex attribute properties
	struct TFBufferPerObjectTerrain
	{
		tfMatrix   worldMatrix;
		tfMatrix   worldInvTransposeMatrix;
		tfMatrix   wvpMatrix;
		tfMatrix   lightVPT;
	};

	class TFTerrain : public TFGrid
	{
	public:
		TFTerrain();
		~TFTerrain();

	void TFTerrain::Init(ID3D11Device* a_pDevice, 
		ID3D11DeviceContext* a_pDeviceContext, 
		ID3D11VertexShader* a_pVertexShader,
		ID3D11PixelShader* a_pPixelShader,
		ID3D11InputLayout* a_pInputLayout,
		const std::string& a_sHeightmap,
		int a_nGridWidth,
		int a_nGridDepth);


	void UpdateResources(const tfMatrix& a_matWVP, 
		const tfMatrix& a_matWorld, 
		const tfMatrix& a_matLightWVPT, 
		const tfFloat3& a_vEyePos);

	void Draw();

	void UpdateFrameData(tfFloat4 a_f4Data);

	// TODO: CLEANUP COM CRAP IN DTOR
	private:
		void GenerateGrid(int a_nWidth, int a_nDepth, float a_fTextureScale);
		
	private:
		ID3D11ShaderResourceView* m_pTexture1SRV;
		ID3D11ShaderResourceView* m_pTexture2SRV;
		ID3D11ShaderResourceView* m_pTexture3SRV;
		ID3D11ShaderResourceView* m_pTexture4SRV;
		ID3D11ShaderResourceView* m_pBlendMapGrassSRV;
		ID3D11ShaderResourceView* m_pBlendMapDirtSRV;
		ID3D11Buffer*             m_pCBPerFrame;
	};

}