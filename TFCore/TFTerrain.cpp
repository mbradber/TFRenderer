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
		ID3D11VertexShader* a_pVertexShader,
		ID3D11PixelShader* a_pPixelShader,
		ID3D11InputLayout* a_pInputLayout,
		const std::string& a_sAssetPath,
		const std::wstring& a_sBlendMap,
		int a_nGridSize)
	{
		m_pd3dDevice = a_pDevice;
		m_pDeviceContext = a_pDeviceContext;

		m_pVertexShader = a_pVertexShader;
		m_pPixelShader = a_pPixelShader;
		m_pInputLayout = a_pInputLayout;
		m_wsBlendMapPath = a_sBlendMap;

		//GenerateHeightMap(a_sAssetPath, a_nGridSize);

		LoadBM("..\\Textures\\Terrain\\heightmap_1.bmp");
		GenerateGrid(a_nGridSize, a_nGridSize, 16.0f);
	}

	void TFTerrain::GenerateHeightMap(std::string a_sFilePath, int a_nGridSize)
	{
		m_hmData.resize(a_nGridSize * a_nGridSize, 0);

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

	int TFTerrain::GetNextValue4B(ifstream& a_bmStream)
	{
		char c[4];
		a_bmStream.read(c, 4);

		int d = (int)((UCHAR)c[3] << 24);
		d    += (int)((UCHAR)c[2] << 16);
		d    += (int)((UCHAR)c[1] << 8);
		d    += (int)((UCHAR)c[0]);

		return d;
	}

	void TFTerrain::LoadBM(const std::string& a_sFilePath)
	{
		ifstream _bmStream(a_sFilePath, ifstream::binary);

		if(_bmStream.is_open())
		{
			char a = _bmStream.get(); // B
			char b = _bmStream.get(); // M

			int c = GetNextValue4B(_bmStream); // bm file size
			int d = GetNextValue4B(_bmStream); // reserved
			size_t e = GetNextValue4B(_bmStream); // raster data offset

			_bmStream.seekg(e + 1, _bmStream.beg); // seek to raster data + 1 (first byte will be 0)

			while(_bmStream.good())
			{
				m_hmData.push_back(_bmStream.get());
				_bmStream.seekg(3, _bmStream.cur); // go forward 3 bytes to the next height value			
			}
		}

		m_hmData.pop_back(); // extraneous 255 at the end...

		_bmStream.close();
	}

	void TFTerrain::GenerateGrid(int a_nWidth, int a_nDepth, float a_fTextureScale)
	{
		int _nFaceCount = (a_nWidth - 1) * (a_nDepth - 1) * 2;
		m_nIndexCount = _nFaceCount * 3;

		std::vector<TFPosNormTex4Tan> _vVertices(a_nWidth * a_nDepth);

		int _nVertIdx = 0;
		for(int i = 0; i < a_nDepth; ++i)
		{
			for(int j = 0; j < a_nWidth; ++j)
			{
				_vVertices[_nVertIdx].Pos.x = (float)j - (a_nWidth / 2);
				_vVertices[_nVertIdx].Pos.y = m_hmData[_nVertIdx] / 2.0f;
				_vVertices[_nVertIdx].Pos.z = (float)i - (a_nDepth / 2);

				_vVertices[_nVertIdx].Norm.x = 0;
				_vVertices[_nVertIdx].Norm.y = 1;
				_vVertices[_nVertIdx].Norm.z = 0;	

				_vVertices[_nVertIdx].TanU.x = 1;
				_vVertices[_nVertIdx].TanU.y = 0;
				_vVertices[_nVertIdx].TanU.z = 0;	

				// set repeating tiles UV coords
				_vVertices[_nVertIdx].TexC.x = (float)j / a_fTextureScale;
				_vVertices[_nVertIdx].TexC.y = (float)((a_nDepth - 1) - i) / a_fTextureScale;

				// Set blend map's UV coords
				_vVertices[_nVertIdx].TexC.z = (float)j / a_nWidth;
				_vVertices[_nVertIdx].TexC.w = (float)((a_nDepth - 1) - i) / a_nDepth;

				_nVertIdx++;
			}
		}

		// Smooth out the vertex heights

		int k = 0;
		for(int i = 0; i < a_nDepth; ++i)
		{
			for(int j = 0; j < a_nWidth; ++j)
			{
				k = (i * a_nWidth) + j; // current index

				// check if this index is on a border
				if(k + a_nWidth >= (int)_vVertices.size() - 1 // top row
					|| k - a_nWidth <= 0                 // bottom row
					|| j + 1 == a_nWidth                 // right border
					|| j == 0                            // left border
					)
				{
					continue;
				}

				else
				{
					float _fTotalHeight = _vVertices[k + a_nWidth + 1].Pos.y;
					_fTotalHeight += _vVertices[k + 1].Pos.y;
					_fTotalHeight += _vVertices[k - a_nWidth + 1].Pos.y;
					_fTotalHeight += _vVertices[k + a_nWidth].Pos.y;
					_fTotalHeight += _vVertices[k].Pos.y;
					_fTotalHeight += _vVertices[k - a_nWidth].Pos.y;
					_fTotalHeight += _vVertices[k + a_nWidth - 1].Pos.y;
					_fTotalHeight += _vVertices[k - 1].Pos.y;
					_fTotalHeight += _vVertices[k - a_nWidth - 1].Pos.y;

					_vVertices[k].Pos.y = (_fTotalHeight / 9.0f);
				}

			}
		}

		// calculate indices

		std::vector<int> _vIndices(m_nIndexCount);

		k = 0;
		for(int i = 0; i < a_nDepth - 1; ++i)
		{
			for(int j = 0; j < a_nWidth - 1; ++j)
			{
				// First triangle of quad...
				_vIndices[k] = (i + 1) * a_nWidth + j;
				_vIndices[k + 1] = (i + 1) * a_nWidth + j + 1;
				_vIndices[k + 2] = i * a_nWidth + j;

				// Calculate normals
				XMVECTOR _vA = XMLoadFloat3(&_vVertices[_vIndices[k]].Pos);
				XMVECTOR _vB = XMLoadFloat3(&_vVertices[_vIndices[k + 1]].Pos);
				XMVECTOR _vC = XMLoadFloat3(&_vVertices[_vIndices[k + 2]].Pos);

				XMVECTOR _vD = _vB - _vA;
				XMVECTOR _vE = _vC - _vA;

				_vE = XMVector3Cross(_vD, _vE);

				// Store the new normal
				XMStoreFloat3(&_vVertices[_vIndices[k]].Norm, _vE);
				XMStoreFloat3(&_vVertices[_vIndices[k + 1]].Norm, _vE);
				XMStoreFloat3(&_vVertices[_vIndices[k + 2]].Norm, _vE);


				// Second triangle of quad...
				_vIndices[k + 3] = i * a_nWidth + j;
				_vIndices[k + 4] = (i + 1) * a_nWidth + j + 1;
				_vIndices[k + 5] = i * a_nWidth + j + 1;

				// Calculate normals
				_vA = XMLoadFloat3(&_vVertices[_vIndices[k + 3]].Pos);
				_vB = XMLoadFloat3(&_vVertices[_vIndices[k + 4]].Pos);
				_vC = XMLoadFloat3(&_vVertices[_vIndices[k + 5]].Pos);	

				_vD = _vB - _vA;
				_vE = _vC - _vA;

				_vE = XMVector3Cross(_vD, _vE);

				// Store the new normal
				XMStoreFloat3(&_vVertices[_vIndices[k + 3]].Norm, _vE);
				XMStoreFloat3(&_vVertices[_vIndices[k + 4]].Norm, _vE);
				XMStoreFloat3(&_vVertices[_vIndices[k + 5]].Norm, _vE);

				// incremenet
				k += 6;
			}
		}



		// describe this buffer
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof( TFPosNormTex4Tan ) * _vVertices.size();
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
		ibd.ByteWidth = sizeof(int) * _vIndices.size();
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

		// Terrain textures

		// grass
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pd3dDevice, 
			L"..\\Textures\\Terrain\\grass.dds", 
			NULL, 
			NULL, 
			&m_pTexture1SRV, 
			NULL));

		// light dirt
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pd3dDevice, 
			L"..\\Textures\\Terrain\\lightdirt.dds", 
			NULL, 
			NULL, 
			&m_pTexture2SRV, 
			NULL));

		// dark dirt
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pd3dDevice, 
			L"..\\Textures\\Terrain\\grass_dull.dds", 
			NULL, 
			NULL, 
			&m_pTexture3SRV, 
			NULL));

		// stone
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pd3dDevice, 
			L"..\\Textures\\Terrain\\darkdirt.dds", 
			NULL, 
			NULL, 
			&m_pTexture4SRV, 
			NULL));

		// load the blend map for the terrain grass
		HR(D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, 
			L"..\\Textures\\Terrain\\blend_map_grass.png",  
			NULL, 
			NULL, 
			&m_pBlendMapGrassSRV, 
			NULL));

		// load the blend map for the terrain rock
		HR(D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, 
			L"..\\Textures\\Terrain\\blend_map_dirt.png", 
			NULL, 
			NULL, 
			&m_pBlendMapDirtSRV, 
			NULL));
	}

	void TFTerrain::AddTexture(const std::string& a_sTexturePath)
	{

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
		cb.texMatrix = XMMatrixTranspose(a_matTex);

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
		UINT _nStride = sizeof(TFPosNormTex4Tan);
		UINT _nOffset = 0;

		// bind vertex buffer to input assembler
		m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &_nStride, &_nOffset);

		// bind index buffer to input assembler
		m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		// Set primitive topology
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Bind terrain textures
		m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture1SRV);
		m_pDeviceContext->PSSetShaderResources(1, 1, &m_pTexture2SRV);
		m_pDeviceContext->PSSetShaderResources(2, 1, &m_pTexture3SRV);
		m_pDeviceContext->PSSetShaderResources(3, 1, &m_pTexture4SRV);

		// bind blend map textures
		m_pDeviceContext->PSSetShaderResources(4, 1, &m_pBlendMapGrassSRV);
		m_pDeviceContext->PSSetShaderResources(5, 1, &m_pBlendMapDirtSRV);

		// Draw self
		m_pDeviceContext->DrawIndexed(m_nIndexCount, 0, 0);
	}

}