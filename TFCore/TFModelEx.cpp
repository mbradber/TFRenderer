#include "TFModelEx.h"
#include <assimp/vector3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "TFUtils.h"

namespace TFRendering
{
	TFModelEx::TFModelEx(ID3D11Device* a_pDevice, 
		ID3D11DeviceContext* a_pDeviceContext,
		const std::string& a_sAssetPath)
		:
	    TFIRenderable(a_pDevice, a_pDeviceContext)
	{
		Init(a_sAssetPath);
	}

	TFModelEx::~TFModelEx()
	{

	}

	void TFModelEx::Init(const std::string& a_sAssetPath)
	{
		// Asset importing
		Assimp::Importer importer;

		// remove point and line primitives
		importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE|aiPrimitiveType_POINT);

		const aiScene* scene = importer.ReadFile(
			a_sAssetPath,
			aiProcess_CalcTangentSpace       | 
			aiProcess_Triangulate            |
			aiProcess_JoinIdenticalVertices  |
			aiProcess_SortByPType            |
			aiProcess_MakeLeftHanded         |
			aiProcess_GenNormals             |
			aiProcess_GenUVCoords            |
			aiProcess_TransformUVCoords      |
			aiProcess_FlipWindingOrder       |
			aiProcess_FlipUVs           
			);

		TF_ASSERT(scene != NULL, FILE_NAME, LINE_NO);

		size_t _nNumMeshes = scene->mNumMeshes;

		aiNode* _root = scene->mRootNode;

		// Find number of vertices and indices of this model
		SurveyNode(scene, _root, &m_nVertexCount, &m_nIndexCount); 

		// allocate data buffers
		tfFloat3* _pPositionBuffer = new tfFloat3[m_nVertexCount];
		tfFloat3* _pNormalBuffer   = new tfFloat3[m_nVertexCount];
		tfFloat2* _pTexCoordBuffer = new tfFloat2[m_nVertexCount];
		tfFloat3* _pTangentBuffer  = new tfFloat3[m_nVertexCount];
		UINT*     _pIndices        = new UINT[m_nIndexCount];

		// Traverse the scene nodes and parse vertex and index data
		size_t _nVertexOffset = 0;
		size_t _nIndexOffset  = 0;

		// Populate the vertex and index buffers with the mesh data of this model
		ProcessNode(scene, _root, _pPositionBuffer, _pNormalBuffer, _pTexCoordBuffer,
			_pTangentBuffer, _pIndices, &_nVertexOffset, &_nIndexOffset);

		// describe the buffers for this model

		// POSITION BUFFER
		{
			D3D11_BUFFER_DESC _posBufferDesc;
			ZeroMemory(&_posBufferDesc, sizeof(_posBufferDesc));
			_posBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
			_posBufferDesc.ByteWidth      = sizeof(tfFloat3) * m_nVertexCount;
			_posBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
			_posBufferDesc.CPUAccessFlags = 0; // No cpu access
			_posBufferDesc.MiscFlags      = 0; // Unused

			D3D11_SUBRESOURCE_DATA _posInitData;
			ZeroMemory( &_posInitData, sizeof(_posInitData) );
			_posInitData.pSysMem = _pPositionBuffer;

			// Create a buffer to hold this model's vert data in video memory
			HR(m_pDevice->CreateBuffer(&_posBufferDesc, &_posInitData, &m_pPositionVertexBuffer));
		}
		
		// NORMAL BUFFER
		{
			D3D11_BUFFER_DESC _normBufferDesc;
			ZeroMemory(&_normBufferDesc, sizeof(_normBufferDesc));
			_normBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
			_normBufferDesc.ByteWidth      = sizeof(tfFloat3) * m_nVertexCount;
			_normBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
			_normBufferDesc.CPUAccessFlags = 0; // No cpu access
			_normBufferDesc.MiscFlags      = 0; // Unused

			D3D11_SUBRESOURCE_DATA _normInitData;
			ZeroMemory(&_normInitData, sizeof(_normInitData));
			_normInitData.pSysMem = _pNormalBuffer;

			// Create a buffer to hold this model's vert data in video memory
			HR(m_pDevice->CreateBuffer(&_normBufferDesc, &_normInitData, &m_pNormalVertexBuffer));
		}

		// TEXCOORD BUFFER
		{
			D3D11_BUFFER_DESC _texcoordBufferDesc;
			ZeroMemory(&_texcoordBufferDesc, sizeof(_texcoordBufferDesc));
			_texcoordBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
			_texcoordBufferDesc.ByteWidth      = sizeof(tfFloat2) * m_nVertexCount;
			_texcoordBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
			_texcoordBufferDesc.CPUAccessFlags = 0; // No cpu access
			_texcoordBufferDesc.MiscFlags      = 0; // Unused

			D3D11_SUBRESOURCE_DATA _texcoordInitData;
			ZeroMemory(&_texcoordInitData, sizeof(_texcoordInitData));
			_texcoordInitData.pSysMem = _pTexCoordBuffer;

			// Create a buffer to hold this model's vert data in video memory
			HR(m_pDevice->CreateBuffer(&_texcoordBufferDesc, &_texcoordInitData, &m_pTexCoordVertexBuffer));
		}

		// TANGENT BUFFER
		{
			D3D11_BUFFER_DESC _tangentBufferDesc;
			ZeroMemory(&_tangentBufferDesc, sizeof(_tangentBufferDesc));
			_tangentBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
			_tangentBufferDesc.ByteWidth      = sizeof(tfFloat3) * m_nVertexCount;
			_tangentBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
			_tangentBufferDesc.CPUAccessFlags = 0; // No cpu access
			_tangentBufferDesc.MiscFlags      = 0; // Unused

			D3D11_SUBRESOURCE_DATA _tangentInitData;
			ZeroMemory(&_tangentInitData, sizeof(_tangentInitData));
			_tangentInitData.pSysMem = _pTangentBuffer;

			// Create a buffer to hold this model's vert data in video memory
			HR(m_pDevice->CreateBuffer(&_tangentBufferDesc, &_tangentInitData, &m_pTangentVertexBuffer));
		}

		// INDEX BUFFER
		{
			D3D11_BUFFER_DESC _indexBufferDesc;
			ZeroMemory(&_indexBufferDesc, sizeof(_indexBufferDesc));

			_indexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
			_indexBufferDesc.ByteWidth      = sizeof(UINT) * m_nIndexCount;
			_indexBufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
			_indexBufferDesc.CPUAccessFlags = 0;
			_indexBufferDesc.MiscFlags      = 0; // Unused

			D3D11_SUBRESOURCE_DATA _indexData;
			ZeroMemory(&_indexData, sizeof(_indexData));
			_indexData.pSysMem  = _pIndices;

			HR(m_pDevice->CreateBuffer(&_indexBufferDesc, &_indexData, &m_pIndexBuffer));
		}

		// deallocate data buffers
		delete[] _pPositionBuffer;
		delete[] _pNormalBuffer;
		delete[] _pTexCoordBuffer;
		delete[] _pTangentBuffer;
		delete[] _pIndices;  
	}

	void TFModelEx::SurveyNode(const aiScene* const a_pScene, aiNode* a_pNode, size_t* a_pNumVerts, size_t* a_pNumIndices)
	{
		// grab the number of meshes for this node
		size_t _nNumMeshes = a_pNode->mNumMeshes;

		// for each mesh, count the number of vertices and indices
		for(size_t i = 0; i < _nNumMeshes; ++i)
		{
			aiMesh* _mesh   = a_pScene->mMeshes[a_pNode->mMeshes[i]];

			// Make sure all the primitive types of this mesh are triangles
			TF_ASSERT(_mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE, FILE_NAME, LINE_NO);

			*a_pNumVerts   += _mesh->mNumVertices;
			*a_pNumIndices += _mesh->mNumFaces * 3;
		}

		// return if this node has no children
		if(a_pNode->mNumChildren == 0)
		{
			return;
		}
		else
		{
			// recursively process each child node of this node
			for(size_t i = 0; i < a_pNode->mNumChildren; ++i)
			{
				SurveyNode(a_pScene, a_pNode->mChildren[i], a_pNumVerts, a_pNumIndices);
			}
		}
	}

	void TFModelEx::ProcessNode(const aiScene* const a_pScene,
		aiNode*	   a_pNode,
		tfFloat3*& a_pPositionBuffer,
		tfFloat3*& a_pNormalBuffer,
		tfFloat2*& a_pTexCoordBuffer,
		tfFloat3*& a_pTangentBuffer,
		UINT*&     a_pIndices,
		UINT*      a_pVertexOffset,
		UINT*      a_pIndexOffset)
	{
		// grab the number of meshes for this node
		size_t _nNumMeshes = a_pNode->mNumMeshes;

		// for each mesh of this node
		for(size_t i = 0; i < _nNumMeshes; ++i)
		{
			aiMesh* _mesh   = a_pScene->mMeshes[a_pNode->mMeshes[i]];

			// Make sure all the primitive types of this mesh are triangles
			TF_ASSERT(_mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE, FILE_NAME, LINE_NO);

			// Process each vertex of this mesh
			for(size_t j = 0; j < _mesh->mNumVertices; ++j)
			{
				// calculate the offset for this vertex of the mesh in the model's single large vertex buffer
				size_t _nVertexIndex = j + *a_pVertexOffset;

				// Copy vertex data
				a_pPositionBuffer[_nVertexIndex].x = _mesh->mVertices[j].x;
				a_pPositionBuffer[_nVertexIndex].y = _mesh->mVertices[j].y;
				a_pPositionBuffer[_nVertexIndex].z = _mesh->mVertices[j].z;

				// normals
				a_pNormalBuffer[_nVertexIndex].x = _mesh->mNormals[j].x;
				a_pNormalBuffer[_nVertexIndex].y = _mesh->mNormals[j].y;
				a_pNormalBuffer[_nVertexIndex].z = _mesh->mNormals[j].z;

				// copy tex coords from mesh
				a_pTexCoordBuffer[_nVertexIndex].x = _mesh->mTextureCoords[0][j].x;
				a_pTexCoordBuffer[_nVertexIndex].y = _mesh->mTextureCoords[0][j].y;

				// copy tangent data from mesh
				if(_mesh->HasTangentsAndBitangents())
				{
					a_pTangentBuffer[_nVertexIndex].x = _mesh->mTangents[j].x;
					a_pTangentBuffer[_nVertexIndex].y = _mesh->mTangents[j].y;
					a_pTangentBuffer[_nVertexIndex].z = _mesh->mTangents[j].z;
				}
			}

			// copy index data
			for(size_t j = 0; j < _mesh->mNumFaces; ++j)
			{
				aiFace _face         = _mesh->mFaces[j];
				// store this mesh's index data in the model's index buffer
				size_t _nIndexOffset = (j * 3) + *a_pIndexOffset;

				a_pIndices[_nIndexOffset + 0] = _face.mIndices[0] + *a_pVertexOffset;
				a_pIndices[_nIndexOffset + 1] = _face.mIndices[1] + *a_pVertexOffset;
				a_pIndices[_nIndexOffset + 2] = _face.mIndices[2] + *a_pVertexOffset;
			}

			// Check this mesh for materials
			aiMaterial* _pMaterial      = a_pScene->mMaterials[_mesh->mMaterialIndex];
			size_t _nNumDiffuseTextures = _pMaterial->GetTextureCount(aiTextureType_DIFFUSE);
			size_t _nNumBumpTextures    = _pMaterial->GetTextureCount(aiTextureType_HEIGHT); //bump map

			aiString _sTexturePathColor;
			aiString _sTexturePathNormal;
			aiReturn _textureFoundColor;
			aiReturn _textureFoundNormal;

			// find diffuse map texture
			for(size_t i = 0; i < _nNumDiffuseTextures; ++i)
			{
				_textureFoundColor = _pMaterial->GetTexture(aiTextureType_DIFFUSE, i, &_sTexturePathColor);

				if(_textureFoundColor == AI_SUCCESS)
				{
					break;
				}
			}

			// find normal map texture
			for(size_t i = 0; i < _nNumBumpTextures; ++i)
			{
				_textureFoundNormal = _pMaterial->GetTexture(aiTextureType_HEIGHT, i, &_sTexturePathNormal);

				if(_textureFoundNormal == AI_SUCCESS)
				{
					break;
				}
			}

			// Convert texture path to widestring
			std::string  _sTexturePathColorAsString  = _sTexturePathColor.C_Str();
			std::string  _sTexturePathNormalAsString = _sTexturePathNormal.C_Str();
			std::wstring _wsTexturePathColor         = L"..\\Textures\\";
			std::wstring _wsTexturePathNormal        = L"..\\Textures\\";

			_wsTexturePathColor.append(std::wstring(_sTexturePathColorAsString.begin(), _sTexturePathColorAsString.end()));
			_wsTexturePathNormal.append(std::wstring(_sTexturePathNormalAsString.begin(), _sTexturePathNormalAsString.end()));

			// Build mesh data
			TFMesh _tfMesh;
			_tfMesh.StartIndex        = *a_pIndexOffset; // the offset in the index buffer where this mesh starts
			_tfMesh.NumIndices        = _mesh->mNumFaces * 3;

			// TODO: D3DX stuff is deprecated, use another method for loading textures when you have time.
			
			// Create Shader resource view from texture path and store it for binding later when drawing this mesh
			if(_nNumDiffuseTextures > 0)
			{
				ID3D11ShaderResourceView* _pTextureSRV;
				HR(D3DX11CreateShaderResourceViewFromFile(m_pDevice, _wsTexturePathColor.c_str(), NULL, NULL, &_pTextureSRV, NULL));
				m_vMeshTexturesColor.push_back(_pTextureSRV);

				// save the index in the diffuse textures vector (m_vMeshTexturesColor) that this meshes diffuse
				// texture is stored
				_tfMesh.TextureIndexColor = m_vMeshTexturesColor.size() - 1; 
			}

			if(_nNumBumpTextures > 0)
			{
				ID3D11ShaderResourceView* _pTextureSRV;
				HR(D3DX11CreateShaderResourceViewFromFile(m_pDevice, _wsTexturePathNormal.c_str(), NULL, NULL, &_pTextureSRV, NULL));
				m_vMeshTexturesNormals.push_back(_pTextureSRV);

				// save the index in the normal map textures vector (m_vMeshTexturesNormals) that this meshes normal
				// map is stored
				_tfMesh.TextureIndexNormal = m_vMeshTexturesNormals.size() - 1;
			}

			// Save mesh data to be draw later
			m_meshes.push_back(_tfMesh);
			
			// Update the vertex offset for the next mesh
			*a_pVertexOffset += _mesh->mNumVertices;

			// Update the index offset for next mesh
			*a_pIndexOffset += (_mesh->mNumFaces * 3);
		}

		// return if this node has no children
		if(a_pNode->mNumChildren == 0)
		{
			return;
		}
		else
		{
			// recursively process each child node of this node
			for(size_t i = 0; i < a_pNode->mNumChildren; ++i)
			{
				ProcessNode(a_pScene, a_pNode->mChildren[i], a_pPositionBuffer, 
					a_pNormalBuffer, a_pTexCoordBuffer, a_pTangentBuffer,
					a_pIndices, a_pVertexOffset, a_pIndexOffset);
			}
		}
	}

	void TFModelEx::Draw()
	{
		// draw each mesh of the model
		for(std::vector<TFMesh>::iterator _itr = m_meshes.begin();
			_itr != m_meshes.end(); 
			++_itr)
		{
			// Bind texture for diffuse map and normal map if they exist for this mesh
			if(_itr->TextureIndexColor != UINT_MAX)
			{
				m_pDeviceContext->PSSetShaderResources(0, 1, &m_vMeshTexturesColor[_itr->TextureIndexColor]);
			}
			if(_itr->TextureIndexNormal != UINT_MAX)
			{
				m_pDeviceContext->PSSetShaderResources(1, 1, &m_vMeshTexturesNormals[_itr->TextureIndexNormal]);
			}

			// Draw the indices of this mesh
			m_pDeviceContext->DrawIndexed(_itr->NumIndices, _itr->StartIndex, 0);
		}
	}

}