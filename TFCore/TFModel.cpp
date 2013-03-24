#include "TFModel.h"
#include "TFUtils.h"
#include "TFVertices.h"
#include <vector>
#include <assimp\vector3.h>


namespace TFCore
{

	TFModel::TFModel()
		:m_nVertexCount(0),
		 m_nIndexCount(0)
	{
		// Define material for cube
		m_material.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		m_material.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_material.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
	}


	TFModel::~TFModel()
	{

	}

	void TFModel::Init(ID3D11Device* a_pDevice, 
		ID3D11DeviceContext* a_pDeviceContext, 
		float a_fScale, 
		ID3D11VertexShader* a_pVertexShader,
		ID3D11PixelShader* a_pPixelShader,
		ID3D11InputLayout* a_pInputLayout,
		const std::string& a_sAssetPath)
	{
		TF_ASSERT(a_pDevice != NULL && a_pDeviceContext != NULL && a_pVertexShader != NULL && a_pPixelShader != NULL,
			FILE_NAME, LINE_NO);

		m_pd3dDevice     = a_pDevice;
		m_pDeviceContext = a_pDeviceContext;
		m_fScale         = a_fScale;
		m_sAssetPath    = a_sAssetPath;
		m_pVertexShader  = a_pVertexShader;
		m_pPixelShader   = a_pPixelShader;
		m_pInputLayout   = a_pInputLayout;

		// Asset importing
		Assimp::Importer importer;

		// remove point and line primitives
		importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE|aiPrimitiveType_POINT);

		const aiScene* scene = importer.ReadFile(
			m_sAssetPath,
			aiProcess_CalcTangentSpace       | 
			aiProcess_Triangulate            |
			aiProcess_JoinIdenticalVertices  |
			aiProcess_SortByPType            |
			aiProcess_MakeLeftHanded         |
			aiProcess_GenSmoothNormals       |
			aiProcess_GenUVCoords            |
			aiProcess_TransformUVCoords      |
			aiProcess_FlipWindingOrder       |
			aiProcess_FlipUVs           
			);

		TF_ASSERT(scene != NULL, FILE_NAME, LINE_NO);


		size_t _nNumMeshes = scene->mNumMeshes;
		//size_t _nNumTextuers = scene->mNumTextures;

		aiNode* _root = scene->mRootNode;

		SurveyNode(scene, _root, &m_nVertexCount, &m_nIndexCount); 

		// Allocate data buffers
		TFPosNormTex* _pVertices = new TFPosNormTex[m_nVertexCount];
		UINT*        _pIndices  = new UINT[m_nIndexCount];

		// Traverse the scene nodes and parse vertex and index data
		size_t _nVertexOffset = 0;
		size_t _nIndexOffset  = 0;
		aiMatrix4x4 _matAccumulation;
		ProcessNode(scene, _root, _pVertices, _pIndices, &_nVertexOffset, &_nIndexOffset);

		// describe the vertex buffer
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof( TFPosNormTex ) * m_nVertexCount;
		bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0; // No cpu access
		bd.MiscFlags      = 0; // Unused

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory( &InitData, sizeof(InitData) );
		InitData.pSysMem = _pVertices;

		// Create a buffer to hold this cube's vert data in video memory
		HR(m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer));

		// describe this index buffer
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof( UINT ) * m_nIndexCount;
		bd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags      = 0; // Unused
		InitData.pSysMem  = _pIndices;
		HR(m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer));


		delete [] _pVertices;
		delete [] _pIndices;


		GenerateShaderResources();
	}

	void TFModel::SurveyNode(const aiScene* const a_pScene, aiNode* a_pNode, size_t* a_pNumVerts, size_t* a_pNumIndices)
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

	void TFModel::ProcessNode(const aiScene* const a_pScene, 
		aiNode* a_pNode, 
		TFPosNormTex*& a_pVertices, 
		UINT*& a_pIndices, 
		size_t* a_pVertexOffset, 
		size_t* a_pIndexOffset)
	{
		// grab the number of meshes for this node
		size_t _nNumMeshes = a_pNode->mNumMeshes;

		for(size_t i = 0; i < _nNumMeshes; ++i)
		{
			aiMesh* _mesh   = a_pScene->mMeshes[a_pNode->mMeshes[i]];

			// Make sure all the primitive types of this mesh are triangles
			TF_ASSERT(_mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE, FILE_NAME, LINE_NO);

			for(size_t j = 0; j < _mesh->mNumVertices; ++j)
			{
				size_t _nVertexIndex = j + *a_pVertexOffset;

				// Copy vertex data
				a_pVertices[_nVertexIndex].Pos.x = _mesh->mVertices[j].x;
				a_pVertices[_nVertexIndex].Pos.y = _mesh->mVertices[j].y;
				a_pVertices[_nVertexIndex].Pos.z = _mesh->mVertices[j].z;

				a_pVertices[_nVertexIndex].Norm.x = _mesh->mNormals[j].x;
				a_pVertices[_nVertexIndex].Norm.y = _mesh->mNormals[j].y;
				a_pVertices[_nVertexIndex].Norm.z = _mesh->mNormals[j].z;

				// copy text from mesh
				a_pVertices[_nVertexIndex].TexC.x = _mesh->mTextureCoords[0][j].x;
				a_pVertices[_nVertexIndex].TexC.y = _mesh->mTextureCoords[0][j].y;
			}

			// copy index data
			for(size_t j = 0; j < _mesh->mNumFaces; ++j)
			{
				aiFace _face         = _mesh->mFaces[j];
				size_t _nNumIndices  = _face.mNumIndices;
				size_t _nIndexOffset = (j * 3) + *a_pIndexOffset;

				a_pIndices[_nIndexOffset + 0] = _face.mIndices[0] + *a_pVertexOffset;
				a_pIndices[_nIndexOffset + 1] = _face.mIndices[1] + *a_pVertexOffset;
				a_pIndices[_nIndexOffset + 2] = _face.mIndices[2] + *a_pVertexOffset;
			}

			// Check this mesh for materials
			aiMaterial* _pMaterial = a_pScene->mMaterials[_mesh->mMaterialIndex];
			size_t _nNumDiffuseTextures = _pMaterial->GetTextureCount(aiTextureType_DIFFUSE);

			aiString _sTexturePath;

			for(size_t i = 0; i < _nNumDiffuseTextures; ++i)
			{
				aiReturn _textureFound = _pMaterial->GetTexture(aiTextureType_DIFFUSE, i, &_sTexturePath);

				if(_textureFound == AI_SUCCESS)
				{
					break;
				}
			}

			// Convert texture path to widestring
			std::string _sTexturePathAsString = _sTexturePath.C_Str();
			m_wsTexturePath = L"..\\Textures\\";
			m_wsTexturePath.append(std::wstring(_sTexturePathAsString.begin(), _sTexturePathAsString.end()));

			// Build mesh data
			TFMesh _tfMesh;
			_tfMesh.TexturePath = m_wsTexturePath;
			_tfMesh.StartIndex  = *a_pIndexOffset;
			_tfMesh.NumIndices  = _mesh->mNumFaces * 3;
			
			// Create Shader resource view from texture path and store it
			HR(D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, m_wsTexturePath.c_str(), NULL, NULL, &m_pTextureSRV, NULL));
			m_vMeshTextures.push_back(m_pTextureSRV);
			_tfMesh.TextureIndex = m_vMeshTextures.size() - 1;

			// Save mesh data
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
				ProcessNode(a_pScene, a_pNode->mChildren[i], a_pVertices, a_pIndices, a_pVertexOffset, a_pIndexOffset);
			}
		}
	}

	// TODO: Look into making this a more general function or moving it into a "Resource Manager". It is not 
	// good practice to have to replace the type everytime we want to use a new vertex type.
	void TFModel::GenerateShaderResources()
	{
		// describe the cb for the WVP matrix
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof(TFBufferPerObject);
		bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags      = 0;

		// Create the constant buffer with the device
		HR(m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pCBPerObject));

		// TODO: D3DX stuff is deprecated, use another method for loading textures when you have time.

		// Create the texture for this model
		//HR(D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, m_wsTexturePath.c_str(), NULL, NULL, &m_pTextureSRV, NULL));
		//HR(D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, , NULL, NULL, &m_pTextureSRV, NULL));
		
	}

	ID3D11VertexShader* TFModel::GetVertexShader() const
	{
		return m_pVertexShader;
	}

	ID3D11PixelShader*  TFModel::GetPixelShader() const
	{
		return m_pPixelShader;
	}

	ID3D11InputLayout* TFModel::GetInputLayout() const
	{
		return m_pInputLayout;
	}

	// TODO: Address tedious type changing issue...
	void TFModel::UpdateResources(const XMMATRIX& a_matWVP, const XMMATRIX& a_matWorld, const XMMATRIX& a_matTex, const XMFLOAT3& a_vEyePos)
	{
		//UPDATE TRANSFORM RESOURCE
		TFCore::TFBufferPerObject cb;

		// update world matrix
		cb.worldMatrix = XMMatrixTranspose(a_matWorld);

		// Update world inverse transpose matrix (used to transform normals as it will be distorted 
		// with non uniform scaling transforms, see pg. 277 of Luna...
		XMMATRIX _wit = a_matWorld;
		_wit.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR _witDet = XMMatrixDeterminant(_wit);

		cb.worldInvTransposeMatrix = XMMatrixInverse(&_witDet, _wit);

		// update wvp of buffer
		cb.wvpMatrix = XMMatrixTranspose(a_matWVP);
		// update the transform matrix for the texture coordinates
		cb.texMatrix = a_matTex;

		//update material of buffer
		cb.material  = m_material;

		m_pDeviceContext->UpdateSubresource(m_pCBPerObject , 0, NULL, &cb, 0, 0);
	}

	// TODO: The slot number argument that is specified when binding these resources corresponds to 
	// the register number used in the shader. This is prone to shader errors and the HLSL reflection API should
	// be used to try and get the name of resources in the shader instead.
	void TFModel::ActivateShaders()
	{
		// Bind shaders
		m_pDeviceContext->VSSetShader(GetVertexShader(), NULL, 0);
		m_pDeviceContext->PSSetShader(GetPixelShader(), NULL, 0);
		// Bind constant buffers to VS and PS
		m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCBPerObject);
		m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pCBPerObject);
		// Bind texture
		//m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTextureSRV);
		// Set the input layout
		m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	}

	// TODO: Its inefficient to be setting all these states per draw call, should 
	// only do it once per batch of renderable types
	void TFModel::Draw()
	{
		// Set vertex buffers
		UINT _nStride = sizeof(TFPosNormTex);
		UINT _nOffset = 0;

		m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &_nStride, &_nOffset);

		// Set index buffer
		m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		// Set primitive topology
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// draw each mesh of the model
		for(std::vector<TFMesh>::iterator _itr = m_meshes.begin();
			_itr != m_meshes.end(); 
			++_itr)
		{
			// Bind texture
			m_pDeviceContext->PSSetShaderResources(0, 1, &m_vMeshTextures[_itr->TextureIndex]);

			// Draw the indices of this mesh
			m_pDeviceContext->DrawIndexed(_itr->NumIndices, _itr->StartIndex, 0);
		}


		//// Draw self
		//m_pDeviceContext->DrawIndexed(m_nIndexCount, 0, 0);
	}

}
