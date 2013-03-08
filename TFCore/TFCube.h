#pragma once

#include <d3d11.h>
#include "TFIRenderable.h"

namespace TFCore
{
	struct TFDirectionalLight
	{
		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;
		XMFLOAT3 Direction;
		float    _pad;

		TFDirectionalLight()
		{
			Ambient   = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
			Diffuse   = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
			Specular  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
			Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
		}
	};

	struct TFMaterial
	{
		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;

		TFMaterial()
		{
			Ambient  = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
			Diffuse  = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
			Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
		}
	};

	// BUFFER OBJECTS

	// Buffer for light object
	struct TFBufferDirectionalLight
	{
		TFDirectionalLight DirLight;
		XMFLOAT3 EyePos;
		float    _pad;
	};

	// Layout of the buffer for world-view-projection matrix
	struct TFBufferWVP
	{
		XMMATRIX   worldMatrix;
		XMMATRIX   worldInvTransposeMatrix;
		XMMATRIX   wvpMatrix;
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
			const std::wstring& a_sFilePath);

		void Draw();

		/*** TFIRenderable methods ***/
		virtual const wchar_t* const GetShaderPath() const;
		virtual const D3D11_INPUT_ELEMENT_DESC* const GetInputLayout() const;
		virtual void SetShaderPath(const std::wstring& a_sFilePath);
		virtual ID3D11VertexShader* GetVertexShader() const; 
		virtual ID3D11PixelShader*  GetPixelShader()  const;
		virtual void UpdateResources(const XMMATRIX& a_matWVP, const XMMATRIX& a_matWorld, const XMFLOAT3& a_vEyePos);
		virtual void ActivateShaders();
		void CompileShaderFromFile(const wchar_t* a_cbFileName, LPCSTR a_pEntryPoint, LPCSTR a_pShaderModel, ID3DBlob** a_ppBlobOut);

	private:

		virtual void GenerateShaders();
		virtual void GenerateShaderResources();

	private:

		ID3D11Device*			  m_pd3dDevice;
		ID3D11DeviceContext*	  m_pDeviceContext;
		ID3D11Buffer*			  m_pVertexBuffer;
		ID3D11Buffer*			  m_pIndexBuffer;
		D3D11_INPUT_ELEMENT_DESC  m_aVertexLayout[2];
		ID3D11VertexShader*	      m_pVertexShader;
		ID3D11PixelShader*		  m_pPixelShader;
		ID3D11ShaderResourceView* m_pCrateTextureSRV;
		ID3D11InputLayout*		  m_pInputLayout;
		ID3D11Buffer*			  m_pConstantBufferWVP;
		ID3D11Buffer*             m_pConstantBufferLight;
		float					  m_fScale;
		std::wstring              m_wsShaderPath;
		const size_t			  VERTEX_COUNT;
		const size_t			  INDEX_COUNT;


		TFDirectionalLight        m_dirLight;
		TFMaterial                m_material;
	};

}



