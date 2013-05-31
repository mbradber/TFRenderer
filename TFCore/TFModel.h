#pragma once

#include <d3d11.h>
#include "TFVertices.h"
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace TFCore
{
	// Layout of the buffer for vertex attribute properties
	struct TFBufferPerObject
	{
		XMMATRIX   worldMatrix;
		XMMATRIX   worldInvTransposeMatrix;
		XMMATRIX   wvpMatrix;
		XMMATRIX   texMatrix;
		XMMATRIX   lightVPT;
	};

	class TFModel
	{
	public:
		TFModel();
		~TFModel();

		void Init(ID3D11Device* a_pDevice, 
			ID3D11DeviceContext* a_pDeviceContext, 
			ID3D11VertexShader* a_pVertexShader,
			ID3D11PixelShader* a_pPixelShader,
			ID3D11InputLayout* a_pInputLayout,
			const std::string& a_sAssetPath);

		ID3D11VertexShader* GetVertexShader() const; 
		ID3D11PixelShader*  GetPixelShader()  const;
		ID3D11InputLayout*  GetInputLayout() const;

		void Draw();
		void ActivateShaders();
		void AddShadowShaders(ID3D11VertexShader* a_pVertexShader,
			ID3D11PixelShader* a_pPixelShader,
			ID3D11InputLayout* a_pInputLayout);
		void SetShadowMap(ID3D11ShaderResourceView* a_pShadowMap, size_t a_nIndex);
		void SetReflectionMap(ID3D11ShaderResourceView* a_pReflectionMap, size_t a_nIndex);
		void UnloadShadowMap(size_t a_nIndex);
		void ActivateShadowShaders();
		void UpdateResources(const XMMATRIX& a_matWVP, 
			const XMMATRIX& a_matWorld, 
			const XMMATRIX& a_matLightWVPT, 
			const XMMATRIX& a_matTex, 
			const XMFLOAT3& a_vEyePos);
		void UpdateShadowResources(const XMMATRIX& a_matWVP);
		ID3D11ShaderResourceView* GetPrimaryTexture() { return m_vMeshTexturesColor[0]; }

		void SetWorldMatrix(const XMMATRIX& a_matWorld);
		const XMMATRIX& GetWorldMatrix() const;

	private:

		struct TFMesh
		{
			size_t StartIndex;
			size_t NumIndices;
			size_t TextureIndexColor;
			size_t TextureIndexNormal;
			std::wstring TexturePathColor;
			std::wstring TexturePathNormal;

			TFMesh():
				TextureIndexColor(UINT_MAX),
				TextureIndexNormal(UINT_MAX),
				StartIndex(0),
				NumIndices(0)
			{}
		};

	private:
			void SurveyNode(const aiScene* const a_pScene, 
				aiNode* a_pNode, 
				size_t* a_pNumVerts, 
				size_t* a_pNumIndices);

			void ProcessNode(const aiScene* const a_pScene, 
				aiNode* a_pNode, 
				TFPosNormTexTan*& a_pVertices, 
				UINT*& a_pIndices, 
				size_t* a_pVertexOffset, 
				size_t* a_pIndexOffset);

			void GenerateShaderResources();

	private:
		ID3D11Device*							m_pd3dDevice;
		ID3D11DeviceContext*					m_pDeviceContext;
		ID3D11Buffer*							m_pVertexBuffer;
		ID3D11Buffer*							m_pIndexBuffer;
		ID3D11InputLayout*						m_pInputLayout;
		ID3D11InputLayout*						m_pInputLayoutShadows;
		ID3D11Buffer*							m_pCBPerObject;
		ID3D11Buffer*							m_pCBPerObject_Shadow;
		size_t									m_nVertexCount;
		size_t									m_nIndexCount;
		std::string								m_sAssetPath;
		ID3D11VertexShader*						m_pVertexShader;
		ID3D11PixelShader*						m_pPixelShader;
		ID3D11VertexShader*						m_pVertexShaderShadows;
		ID3D11PixelShader*						m_pPixelShaderShadows;
		ID3D11ShaderResourceView*			    m_pTextureSRV;
		std::vector<TFMesh>                     m_meshes;
		std::vector<ID3D11ShaderResourceView*>  m_vMeshTexturesColor;
		std::vector<ID3D11ShaderResourceView*>  m_vMeshTexturesNormals;
		XMMATRIX                                m_matWorld;
	};

}

