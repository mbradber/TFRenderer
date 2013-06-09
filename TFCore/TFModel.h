#pragma once

#include <d3d11.h>
#include "TFVertices.h"
#include <string>
#include <vector>

struct aiScene;
struct aiNode;

namespace TFCore
{
	// Layout of the buffer for vertex attribute properties
	struct TFBufferPerObject
	{
		tfMatrix   worldMatrix;
		tfMatrix   worldInvTransposeMatrix;
		tfMatrix   wvpMatrix;
		tfMatrix   texMatrix;
		tfMatrix   lightVPT;
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
		void UnloadShadowMap(size_t a_nIndex);
		void ActivateShadowShaders();
		void UpdateResources(const tfMatrix& a_matWVP, 
			const tfMatrix& a_matWorld, 
			const tfMatrix& a_matLightWVPT, 
			const tfMatrix& a_matTex, 
			const tfFloat3& a_vEyePos);
		void UpdateShadowResources(const tfMatrix& a_matWVP);
		ID3D11ShaderResourceView* GetPrimaryTexture() { return m_vMeshTexturesColor[0]; }

		void SetWorldMatrix(const tfMatrix& a_matWorld);
		const tfMatrix& GetWorldMatrix() const;

	private:

		struct TFMesh
		{
			size_t StartIndex;
			size_t NumIndices;
			size_t TextureIndexColor;
			size_t TextureIndexNormal;

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

			//void ProcessNode(const aiScene* const a_pScene, 
			//	aiNode* a_pNode, 
			//	TFPosNormTexTan*& a_pVertices, 
			//	UINT*& a_pIndices, 
			//	size_t* a_pVertexOffset, 
			//	size_t* a_pIndexOffset);

			void ProcessNode(const aiScene* const a_pScene,
				aiNode* a_pNode,
				tfFloat3*& a_pPositionBuffer,
				tfFloat3*& a_pNormalBuffer,
				tfFloat2*& a_pTexCoordBuffer,
				tfFloat3*& a_pTangentBuffer,
				UINT*&     a_pIndices,
				UINT*      a_pVertexOffset,
				UINT*      a_pIndexOffset);

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
		std::vector<TFMesh>                     m_meshes;
		std::vector<ID3D11ShaderResourceView*>  m_vMeshTexturesColor;
		std::vector<ID3D11ShaderResourceView*>  m_vMeshTexturesNormals;
		tfMatrix                                m_matWorld;


		ID3D11Buffer* m_pPositionVertexBuffer;
		ID3D11Buffer* m_pNormalVertexBuffer;
		ID3D11Buffer* m_pTexCoordVertexBuffer;
		ID3D11Buffer* m_pTangentVertexBuffer;
	};

}

