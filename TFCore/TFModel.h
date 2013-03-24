#pragma once

#include "TFMaterial.h"
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
		TFMaterial material;
	};

	class TFModel
	{
	public:
		TFModel();
		~TFModel();

		void Init(ID3D11Device* a_pDevice, 
			ID3D11DeviceContext* a_pDeviceContext, 
			float a_fScale, 
			ID3D11VertexShader* a_pVertexShader,
			ID3D11PixelShader* a_pPixelShader,
			ID3D11InputLayout* a_pInputLayout,
			const std::string& a_sAssetPath);

		ID3D11VertexShader* GetVertexShader() const; 
		ID3D11PixelShader*  GetPixelShader()  const;
		ID3D11InputLayout*  GetInputLayout() const;

		void Draw();
		void ActivateShaders();
		void UpdateResources(const XMMATRIX& a_matWVP, const XMMATRIX& a_matWorld, const XMMATRIX& a_matTex, const XMFLOAT3& a_vEyePos);

	private:

		struct TFMesh
		{
			size_t StartIndex;
			size_t NumIndices;
			size_t TextureIndex;
			std::wstring TexturePath;
		};

	private:
			void SurveyNode(const aiScene* const a_pScene, 
				aiNode* a_pNode, 
				size_t* a_pNumVerts, 
				size_t* a_pNumIndices);

			void ProcessNode(const aiScene* const a_pScene, 
				aiNode* a_pNode, 
				TFPosNormTex*& a_pVertices, 
				UINT*& a_pIndices, 
				size_t* a_pVertexOffset, 
				size_t* a_pIndexOffset);

			void GenerateShaderResources();

	private:
		ID3D11Device*			  m_pd3dDevice;
		ID3D11DeviceContext*	  m_pDeviceContext;
		ID3D11Buffer*			  m_pVertexBuffer;
		ID3D11Buffer*			  m_pIndexBuffer;
		ID3D11InputLayout*		  m_pInputLayout;
		ID3D11Buffer*			  m_pCBPerObject;
		float					  m_fScale;
		size_t                    m_nVertexCount;
		size_t                    m_nIndexCount;
		TFMaterial                m_material;
		std::wstring              m_wsShaderPath;
		std::string               m_sAssetPath;
		ID3D11VertexShader*	      m_pVertexShader;
		ID3D11PixelShader*		  m_pPixelShader;
		ID3D11ShaderResourceView* m_pTextureSRV;
		std::wstring              m_wsTexturePath;
		std::vector<TFMesh>       m_meshes;
		std::vector<ID3D11ShaderResourceView*> m_vMeshTextures;
	};

}

