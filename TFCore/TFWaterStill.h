#pragma once

#include "TFGrid.h"

namespace TFCore
{

	// Layout of the buffer for vertex attribute properties
	struct TFBufferPerObjectWater
	{
		tfMatrix   worldMatrix;
		tfMatrix   worldInvTransposeMatrix;
		tfMatrix   wvpMatrix;
		tfMatrix   TexTransform;
		tfMatrix   TexTransformNeg;
	};

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

		void Update(float a_fDelta);
		void BindReflectionMap(ID3D11ShaderResourceView*);
		void ActivateShaders();
		tfMatrix GetTextureTransform() const;
		tfMatrix GetTextureTransformNeg() const;


	void UpdateResources(const tfMatrix& a_matWVP, 
		const tfMatrix& a_matWorld, 
		const tfMatrix& a_matTexTransform,
		const tfMatrix& a_matTexTransformNeg);

	private:
		ID3D11ShaderResourceView* m_pNormalMapSRV;
		tfMatrix m_matTexTransform;
		tfMatrix m_matTexTransformNeg;
		float m_fWaterOffsetX;
		float m_fWaterOffsetY;
	};

}

