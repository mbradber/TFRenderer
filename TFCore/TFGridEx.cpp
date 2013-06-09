#include "TFGridEx.h"
#include <fstream>
#include "TFUtils.h"

namespace TFRendering
{

	using namespace std;
	using namespace DirectX;

	TFGridEx::TFGridEx(ID3D11Device* a_pDevice, 
		ID3D11DeviceContext* a_pDeviceContext,
		const std::string& a_sHeightmap,
		int a_nGridWidth,
		int a_nGridDepth)
		:
	    TFIRenderable(a_pDevice, a_pDeviceContext),
		m_bUsingHeightmap(true)
	{

	}


	TFGridEx::~TFGridEx()
	{

	}

	// utility function to handle little endian data
	int TFGridEx::GetNextValue4B(ifstream& a_bmStream)
	{
		char c[4];
		a_bmStream.read(c, 4);

		int d = (int)((UCHAR)c[3] << 24);
		d    += (int)((UCHAR)c[2] << 16);
		d    += (int)((UCHAR)c[1] << 8);
		d    += (int)((UCHAR)c[0]);

		return d;
	}

	void TFGridEx::GenerateHeightmapFromBMP(const std::string& a_sFilePath)
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

	void TFGridEx::GenerateGrid(int a_nWidth, int a_nDepth, float a_fTextureScale)
	{
		int _nFaceCount = (a_nWidth - 1) * (a_nDepth - 1) * 2;
		m_nIndexCount = _nFaceCount * 3;
		m_nVertexCount = a_nWidth * a_nDepth;
		
		std::vector<tfFloat3> _vPositionBuffer(m_nVertexCount);
		std::vector<tfFloat3> _vNormalBuffer(m_nVertexCount);
		std::vector<tfFloat3> _vTangentBuffer(m_nVertexCount);
		std::vector<tfFloat4> _vTexCoordBuffer(m_nVertexCount);

		int _nVertIdx = 0;
		for(int i = 0; i < a_nDepth; ++i)
		{
			for(int j = 0; j < a_nWidth; ++j)
			{
				//_vVertices[_nVertIdx].Pos.x = (float)j - (a_nWidth / 2);
				_vPositionBuffer[_nVertIdx].x = (float)j - (a_nWidth / 2);

				// determine whether to read height from heightmap
				if(m_bUsingHeightmap)
				{
					//_vVertices[_nVertIdx].Pos.y = m_hmData[_nVertIdx] / 2.0f;
					_vPositionBuffer[_nVertIdx].y = m_hmData[_nVertIdx] / 2.0f;
				}
				else
				{
					//_vVertices[_nVertIdx].Pos.y = 0;
					_vPositionBuffer[_nVertIdx].y = 0;
				}

				//_vVertices[_nVertIdx].Pos.z = (float)i - (a_nDepth / 2);
				_vPositionBuffer[_nVertIdx].z = (float)i - (a_nDepth / 2);

				//_vVertices[_nVertIdx].Norm.x = 0;
				//_vVertices[_nVertIdx].Norm.y = 1;
				//_vVertices[_nVertIdx].Norm.z = 0;	

				_vNormalBuffer[_nVertIdx].x = 0;
				_vNormalBuffer[_nVertIdx].y = 1;
				_vNormalBuffer[_nVertIdx].z = 0;	

				//_vVertices[_nVertIdx].TanU.x = 1;
				//_vVertices[_nVertIdx].TanU.y = 0;
				//_vVertices[_nVertIdx].TanU.z = 0;	

				_vTangentBuffer[_nVertIdx].x = 1;
				_vTangentBuffer[_nVertIdx].y = 0;
				_vTangentBuffer[_nVertIdx].z = 0;	

				// set repeating tiles UV coords
				//_vVertices[_nVertIdx].TexC.x = (float)j / a_fTextureScale;
				//_vVertices[_nVertIdx].TexC.y = (float)((a_nDepth - 1) - i) / a_fTextureScale;

				_vTexCoordBuffer[_nVertIdx].x = (float)j / a_fTextureScale;
				_vTexCoordBuffer[_nVertIdx].y = (float)((a_nDepth - 1) - i) / a_fTextureScale;

				// Set blend map's UV coords
				//_vVertices[_nVertIdx].TexC.z = (float)j / a_nWidth;
				//_vVertices[_nVertIdx].TexC.w = (float)((a_nDepth - 1) - i) / a_nDepth;

				_vTexCoordBuffer[_nVertIdx].z = (float)j / a_nWidth;
				_vTexCoordBuffer[_nVertIdx].w = (float)((a_nDepth - 1) - i) / a_nDepth;

				_nVertIdx++;
			}
		}

		// Smooth out the vertex heights if heights read from heightmap

		int k = 0;
		if(m_bUsingHeightmap)
		{
			for(int i = 0; i < a_nDepth; ++i)
			{
				for(int j = 0; j < a_nWidth; ++j)
				{
					k = (i * a_nWidth) + j; // current index

					// check if this index is on a border
					if(k + a_nWidth >= (int)_vPositionBuffer.size() - 1 // top row
						|| k - a_nWidth <= 0                 // bottom row
						|| j + 1 == a_nWidth                 // right border
						|| j == 0                            // left border
						)
					{
						continue;
					}

					else
					{
						//float _fTotalHeight = _vVertices[k + a_nWidth + 1].Pos.y;
						//_fTotalHeight += _vVertices[k + 1].Pos.y;
						//_fTotalHeight += _vVertices[k - a_nWidth + 1].Pos.y;
						//_fTotalHeight += _vVertices[k + a_nWidth].Pos.y;
						//_fTotalHeight += _vVertices[k].Pos.y;
						//_fTotalHeight += _vVertices[k - a_nWidth].Pos.y;
						//_fTotalHeight += _vVertices[k + a_nWidth - 1].Pos.y;
						//_fTotalHeight += _vVertices[k - 1].Pos.y;
						//_fTotalHeight += _vVertices[k - a_nWidth - 1].Pos.y;

						//_vVertices[k].Pos.y = (_fTotalHeight / 9.0f);

						float _fTotalHeight = _vPositionBuffer[k + a_nWidth + 1].y;
						_fTotalHeight += _vPositionBuffer[k + 1].y;
						_fTotalHeight += _vPositionBuffer[k - a_nWidth + 1].y;
						_fTotalHeight += _vPositionBuffer[k + a_nWidth].y;
						_fTotalHeight += _vPositionBuffer[k].y;
						_fTotalHeight += _vPositionBuffer[k - a_nWidth].y;
						_fTotalHeight += _vPositionBuffer[k + a_nWidth - 1].y;
						_fTotalHeight += _vPositionBuffer[k - 1].y;
						_fTotalHeight += _vPositionBuffer[k - a_nWidth - 1].y;

						_vPositionBuffer[k].y = (_fTotalHeight / 9.0f);
					}

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
				//tfVector _vA = XMLoadFloat3(&_vVertices[_vIndices[k]].Pos);
				//tfVector _vB = XMLoadFloat3(&_vVertices[_vIndices[k + 1]].Pos);
				//tfVector _vC = XMLoadFloat3(&_vVertices[_vIndices[k + 2]].Pos);

				tfVector _vA = XMLoadFloat3(&_vPositionBuffer[_vIndices[k]]);
				tfVector _vB = XMLoadFloat3(&_vPositionBuffer[_vIndices[k + 1]]);
				tfVector _vC = XMLoadFloat3(&_vPositionBuffer[_vIndices[k + 2]]);

				tfVector _vD = _vB - _vA;
				tfVector _vE = _vC - _vA;

				_vE = XMVector3Cross(_vD, _vE);

				// Store the new normal
				//XMStoreFloat3(&_vVertices[_vIndices[k]].Norm, _vE);
				//XMStoreFloat3(&_vVertices[_vIndices[k + 1]].Norm, _vE);
				//XMStoreFloat3(&_vVertices[_vIndices[k + 2]].Norm, _vE);
				XMStoreFloat3(&_vNormalBuffer[_vIndices[k]], _vE);
				XMStoreFloat3(&_vNormalBuffer[_vIndices[k + 1]], _vE);
				XMStoreFloat3(&_vNormalBuffer[_vIndices[k + 2]], _vE);


				// Second triangle of quad...
				_vIndices[k + 3] = i * a_nWidth + j;
				_vIndices[k + 4] = (i + 1) * a_nWidth + j + 1;
				_vIndices[k + 5] = i * a_nWidth + j + 1;

				// Calculate normals
				//_vA = XMLoadFloat3(&_vVertices[_vIndices[k + 3]].Pos);
				//_vB = XMLoadFloat3(&_vVertices[_vIndices[k + 4]].Pos);
				//_vC = XMLoadFloat3(&_vVertices[_vIndices[k + 5]].Pos);	
				_vA = XMLoadFloat3(&_vPositionBuffer[_vIndices[k + 3]]);
				_vB = XMLoadFloat3(&_vPositionBuffer[_vIndices[k + 4]]);
				_vC = XMLoadFloat3(&_vPositionBuffer[_vIndices[k + 5]]);	

				_vD = _vB - _vA;
				_vE = _vC - _vA;

				_vE = XMVector3Cross(_vD, _vE);

				// Store the new normal
				//XMStoreFloat3(&_vVertices[_vIndices[k + 3]].Norm, _vE);
				//XMStoreFloat3(&_vVertices[_vIndices[k + 4]].Norm, _vE);
				//XMStoreFloat3(&_vVertices[_vIndices[k + 5]].Norm, _vE);
				XMStoreFloat3(&_vNormalBuffer[_vIndices[k + 3]], _vE);
				XMStoreFloat3(&_vNormalBuffer[_vIndices[k + 4]], _vE);
				XMStoreFloat3(&_vNormalBuffer[_vIndices[k + 5]], _vE);

				// incremenet
				k += 6;
			}
		}

		// describe this buffer
		//D3D11_BUFFER_DESC bd;
		//ZeroMemory( &bd, sizeof(bd) );
		//bd.Usage          = D3D11_USAGE_DEFAULT;
		//bd.ByteWidth      = sizeof( TFPosNormTex4Tan ) * _vVertices.size();
		//bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
		//bd.CPUAccessFlags = 0; // No cpu access
		//bd.MiscFlags      = 0; // Unused

		//D3D11_SUBRESOURCE_DATA InitData;
		//ZeroMemory( &InitData, sizeof(InitData) );
		//InitData.pSysMem = &_vVertices[0];

		//// Create a buffer to hold this grid's vert data in video memory
		//HR(m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer));



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
			_posInitData.pSysMem = &_vPositionBuffer[0];

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
			_normInitData.pSysMem = &_vNormalBuffer[0];

			// Create a buffer to hold this model's vert data in video memory
			HR(m_pDevice->CreateBuffer(&_normBufferDesc, &_normInitData, &m_pNormalVertexBuffer));
		}

		// TEXCOORD BUFFER
		{
			D3D11_BUFFER_DESC _texcoordBufferDesc;
			ZeroMemory(&_texcoordBufferDesc, sizeof(_texcoordBufferDesc));
			_texcoordBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
			_texcoordBufferDesc.ByteWidth      = sizeof(tfFloat4) * m_nVertexCount;
			_texcoordBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
			_texcoordBufferDesc.CPUAccessFlags = 0; // No cpu access
			_texcoordBufferDesc.MiscFlags      = 0; // Unused

			D3D11_SUBRESOURCE_DATA _texcoordInitData;
			ZeroMemory(&_texcoordInitData, sizeof(_texcoordInitData));
			_texcoordInitData.pSysMem = &_vTexCoordBuffer[0];

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
			_tangentInitData.pSysMem = &_vTangentBuffer[0];

			// Create a buffer to hold this model's vert data in video memory
			HR(m_pDevice->CreateBuffer(&_tangentBufferDesc, &_tangentInitData, &m_pTangentVertexBuffer));
		}

		// create index buffer for grid

		D3D11_BUFFER_DESC ibd;
		ZeroMemory(&ibd, sizeof(ibd));
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(int) * _vIndices.size();
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = &_vIndices[0];

		HR(m_pDevice->CreateBuffer(&ibd, &iinitData, &m_pIndexBuffer));

		// GENERATE SHADER RESOURCES

		//// describe the cb for the WVP matrix for shadow mapping
		//ZeroMemory(&bd, sizeof(bd));
		//bd.Usage          = D3D11_USAGE_DEFAULT;
		//bd.ByteWidth      = sizeof(tfMatrix);
		//bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		//bd.CPUAccessFlags = 0;
		//bd.MiscFlags      = 0;

		//// Create the constant buffer with the device
		//HR(m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pCBPerObject_Shadow));		
	}

}