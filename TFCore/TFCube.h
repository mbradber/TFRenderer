#pragma once

#include <d3d11.h>
#include "TFIRenderable.h"
#include "TFMaterial.h"

namespace TFCore
{
	// Layout of the buffer for world-view-projection matrix
	struct TFBufferWVP
	{
		XMMATRIX   worldMatrix;
		XMMATRIX   worldInvTransposeMatrix;
		XMMATRIX   wvpMatrix;
		XMMATRIX   texMatrix;
		TFMaterial material;
	};

	class TFCube
	{
	public:
		TFCube();
		~TFCube();

		void Init(ID3D11Device* a_pDevice, 
			ID3D11DeviceContext* a_pDeviceContext, 
			float a_fScale, 
			ID3D11VertexShader* a_pVertexShader,
			ID3D11PixelShader* a_pPixelShader,
			ID3D11InputLayout* a_pInputLayout,
			const std::wstring& a_sFilePathTexture);

		void Draw();

		/*** TFIRenderable methods ***/
		virtual const wchar_t* const GetShaderPath() const;
		virtual void SetShaderPath(const std::wstring& a_sFilePath);
		virtual ID3D11VertexShader* GetVertexShader() const; 
		virtual ID3D11PixelShader*  GetPixelShader()  const;
		virtual void UpdateResources(const XMMATRIX& a_matWVP, const XMMATRIX& a_matWorld, const XMMATRIX& a_matTex, const XMFLOAT3& a_vEyePos);
		virtual void ActivateShaders();

	private:

		virtual void GenerateShaderResources();

	private:

		ID3D11Device*			  m_pd3dDevice;
		ID3D11DeviceContext*	  m_pDeviceContext;
		ID3D11Buffer*			  m_pVertexBuffer;
		ID3D11Buffer*			  m_pIndexBuffer;
		ID3D11VertexShader*	      m_pVertexShader;
		ID3D11PixelShader*		  m_pPixelShader;
		ID3D11ShaderResourceView* m_pCrateTextureSRV;
		ID3D11InputLayout*		  m_pInputLayout;
		ID3D11Buffer*			  m_pConstantBufferWVP;
		ID3D11Buffer*             m_pConstantBufferLight;
		TFMaterial                m_material;
		float					  m_fScale;
		std::wstring              m_wsShaderPath;
		std::wstring              m_wsTexturePath;
		const size_t			  VERTEX_COUNT;
		const size_t			  INDEX_COUNT;
	};

}



