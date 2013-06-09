#pragma once

#include <d3d11.h>
#include <string>
#include <xnamath.h>
#include <vector>
#include "TFIRenderable.h"

struct aiScene;
struct aiNode;

namespace TFRendering
{
	class TFModelEx : public TFIRenderable
	{
	public:

		friend class TFEffect;

		TFModelEx(ID3D11Device* a_pDevice, 
			ID3D11DeviceContext* a_pDeviceContext,
			const std::string& a_sAssetPath);

		void Init(const std::string& a_sAssetPath);
		void Draw();


		~TFModelEx();

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

		UINT									m_nVertexCount;
		UINT									m_nIndexCount;
		std::vector<ID3D11ShaderResourceView*>  m_vMeshTexturesColor;
		std::vector<ID3D11ShaderResourceView*>  m_vMeshTexturesNormals;
		std::vector<TFMesh>                     m_meshes;
	};
}