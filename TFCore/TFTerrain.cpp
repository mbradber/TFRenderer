#include "TFTerrain.h"
#include <fstream>
#include <vector>
#include "TFVertices.h"
#include "TFUtils.h"

using namespace std;

namespace TFCore
{

	TFTerrain::TFTerrain()
		:m_nIndexCount(0)
	{
		// Define material for terrain
		m_material.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		m_material.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_material.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
	}


	TFTerrain::~TFTerrain()
	{
	}

	void TFTerrain::Init(ID3D11Device* a_pDevice, 
		ID3D11DeviceContext* a_pDeviceContext, 
		float a_fScale, 
		ID3D11VertexShader* a_pVertexShader,
		ID3D11PixelShader* a_pPixelShader,
		ID3D11InputLayout* a_pInputLayout,
		const std::string& a_sAssetPath)
	{
		m_pd3dDevice = a_pDevice;
		m_pDeviceContext = a_pDeviceContext;

		m_pVertexShader = a_pVertexShader;
		m_pPixelShader = a_pPixelShader;
		m_pInputLayout = a_pInputLayout;

		GenerateHeightMap(a_sAssetPath);

		GenerateGrid(257, 257);
	}

	void TFTerrain::GenerateHeightMap(std::string a_sFilePath)
	{
		//std::vector<unsigned char> m_hmData(2049 * 2049);
		m_hmData.resize(257 * 257, 0);

		ifstream _terrainStream(a_sFilePath, ifstream::binary);

		if(_terrainStream.is_open())
		{
			_terrainStream.read((char*)&m_hmData[0], (std::streamsize)m_hmData.size());
			_terrainStream.close();
		}
		else
		{
			
		}

	}

	void TFTerrain::GenerateGrid(size_t a_nWidth, size_t a_nDepth)
	{
		size_t _nFaceCount = (a_nWidth - 1) * (a_nDepth - 1) * 2;
		m_nIndexCount = _nFaceCount * 3;

		std::vector<TFPosNormTexTan> _vVertices(a_nWidth * a_nDepth);

		size_t _nVertIdx = 0;
		for(size_t i = 0; i < a_nDepth; ++i)
		{
			for(size_t j = 0; j < a_nWidth; ++j)
			{
				_vVertices[_nVertIdx].Pos.x = (float)j - (a_nWidth / 2);
				_vVertices[_nVertIdx].Pos.y = m_hmData[_nVertIdx];
				_vVertices[_nVertIdx].Pos.z = (float)i - (a_nDepth / 2);

				_vVertices[_nVertIdx].Norm.x = 0;
				_vVertices[_nVertIdx].Norm.y = 1;
				_vVertices[_nVertIdx].Norm.z = 0;	

				_vVertices[_nVertIdx].TanU.x = 1;
				_vVertices[_nVertIdx].TanU.y = 0;
				_vVertices[_nVertIdx].TanU.z = 0;	

				_nVertIdx++;
			}
		}

		std::vector<size_t> _vIndices(m_nIndexCount);

		size_t k = 0;
		for(size_t i = 0; i < a_nDepth - 1; ++i)
		{
			for(size_t j = 0; j < a_nWidth - 1; ++j)
			{
				_vIndices[k] = (i + 1) * a_nWidth + j;
				_vIndices[k + 1] = (i + 1) * a_nWidth + j + 1;
				_vIndices[k + 2] = i * a_nWidth + j;

				_vIndices[k + 3] = i * a_nWidth + j;
				_vIndices[k + 4] = (i + 1) * a_nWidth + j + 1;
				_vIndices[k + 5] = i * a_nWidth + j + 1;

				k += 6;
			}
		}

		// describe this buffer
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof( TFPosNormTexTan ) * _vVertices.size();
		bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0; // No cpu access
		bd.MiscFlags      = 0; // Unused

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory( &InitData, sizeof(InitData) );
		InitData.pSysMem = &_vVertices[0];

		// Create a buffer to hold this cube's vert data in video memory
		HR(m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer));

		D3D11_BUFFER_DESC ibd;
		ZeroMemory(&ibd, sizeof(ibd));
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(size_t) * _vIndices.size();
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = &_vIndices[0];

		HR(m_pd3dDevice->CreateBuffer(&ibd, &iinitData, &m_pIndexBuffer));

		// GENERATE SHADER RESOURCES

		// describe the cb for the WVP matrix
		D3D11_BUFFER_DESC sbd;
		ZeroMemory(&sbd, sizeof(sbd));
		sbd.Usage          = D3D11_USAGE_DEFAULT;
		sbd.ByteWidth      = sizeof(TFBufferPerObject);
		sbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		sbd.CPUAccessFlags = 0;
		sbd.MiscFlags      = 0;

		// Create the constant buffer with the device
		HR(m_pd3dDevice->CreateBuffer(&sbd, NULL, &m_pCBPerObject));
	}

	// TODO: The slot number argument that is specified when binding these resources corresponds to 
	// the register number used in the shader. This is prone to shader errors and the HLSL reflection API should
	// be used to try and get the name of resources in the shader instead.
	void TFTerrain::ActivateShaders()
	{
		// Bind shaders
		m_pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
		m_pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);
		// Bind constant buffers to VS and PS
		m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCBPerObject);
		m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pCBPerObject);
		// Bind texture
		//m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTextureSRV);
		// Set the input layout
		m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	}

		// TODO: Address tedious type changing issue...
	void TFTerrain::UpdateResources(const XMMATRIX& a_matWVP, const XMMATRIX& a_matWorld, const XMMATRIX& a_matLightWVPT, const XMMATRIX& a_matTex, const XMFLOAT3& a_vEyePos)
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

		// update matrix to transform from object to projective texture coords
		cb.lightVPT = XMMatrixTranspose(a_matLightWVPT);

		//update material of buffer
		cb.material  = m_material;

		m_pDeviceContext->UpdateSubresource(m_pCBPerObject , 0, NULL, &cb, 0, 0);
	}

	// TODO: Its inefficient to be setting all these states per draw call, should 
	// only do it once per batch of renderable types
	void TFTerrain::Draw()
	{
		// Set vertex buffers
		UINT _nStride = sizeof(TFPosNormTexTan);
		UINT _nOffset = 0;

		// bind vertex buffer to input assembler
		m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &_nStride, &_nOffset);

		// bind index buffer to input assembler
		m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		// Set primitive topology
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// Draw self
		m_pDeviceContext->DrawIndexed(m_nIndexCount, 0, 0);
	}

}