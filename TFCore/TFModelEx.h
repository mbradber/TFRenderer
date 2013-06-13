#pragma once

/*** Concrete renderable class encapsulating 3D models. Utilizies the Open Asset Import Library
	 API to populate vertex and index buffers and create shader resource views from asset 
	 textures such as normal and diffuse maps. Also implements the virtual Draw() method
	 to provide custom drawing functionality for this renderable type ***/

#include <d3d11.h>
#include <string>
#include "TFTypes.h"
#include <vector>
#include "TFIRenderable.h"

struct aiScene;
struct aiNode;

namespace TFRendering
{
	class TFModelEx : public TFIRenderable
	{
	public:

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
			tfFloat3*& a_pPositionBuffer,
			tfFloat3*& a_pNormalBuffer,
			tfFloat2*& a_pTexCoordBuffer,
			tfFloat3*& a_pTangentBuffer,
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

		std::vector<ID3D11ShaderResourceView*>  m_vMeshTexturesColor;
		std::vector<ID3D11ShaderResourceView*>  m_vMeshTexturesNormals;
		std::vector<TFMesh>                     m_meshes;
	};
}