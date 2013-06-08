#pragma once

#include <d3d11.h>
#include <string>
#include <xnamath.h>
#include <vector>

struct aiScene;
struct aiNode;

namespace TFRendering
{
	class TFModelEx
	{
	public:

		TFModelEx();
		~TFModelEx();

		void Init(ID3D11Device* a_pDevice, 
			ID3D11DeviceContext* a_pDeviceContext,
			const std::string& a_sAssetPath);

	private:
		TFModelEx(TFModelEx&); // disable copying
		TFModelEx& operator=(TFModelEx&);

		void SurveyNode(const aiScene* const a_pScene, 
			aiNode* a_pNode, 
			size_t* a_pNumVerts, 
			size_t* a_pNumIndices);

		void ProcessNode(const aiScene* const a_pScene,
			aiNode* a_pNode,
			XMFLOAT3*& a_pPositionBuffer,
			XMFLOAT3*& a_pNormalBuffer,
			XMFLOAT2*& a_pTexCoordBuffer,
			XMFLOAT3*& a_pTangentBuffer,
			UINT*&     a_pIndices,
			UINT*      a_pVertexOffset,
			UINT*      a_pIndexOffset);

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
		ID3D11Device*							m_pDevice;
		ID3D11DeviceContext*					m_pDeviceContext;
		UINT									m_nVertexCount;
		UINT									m_nIndexCount;
		std::vector<ID3D11ShaderResourceView*>  m_vMeshTexturesColor;
		std::vector<ID3D11ShaderResourceView*>  m_vMeshTexturesNormals;
		XMMATRIX                                m_matWorld;
		std::vector<TFMesh>                     m_meshes;

		// buffers
		ID3D11Buffer* m_pPositionVertexBuffer;
		ID3D11Buffer* m_pNormalVertexBuffer;
		ID3D11Buffer* m_pTexCoordVertexBuffer;
		ID3D11Buffer* m_pTangentVertexBuffer;
	};
}