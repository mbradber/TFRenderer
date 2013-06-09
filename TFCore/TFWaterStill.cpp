#include "TFWaterStill.h"
#include <math.h>
#include "TFUtils.h"

namespace TFCore
{
	using namespace DirectX;

	TFWaterStill::TFWaterStill()
		:m_fWaterOffsetX(0),
		 m_fWaterOffsetY(0)
	{
		m_matTexTransform = XMMatrixIdentity();
	}


	TFWaterStill::~TFWaterStill()
	{

	}

	void TFWaterStill::Init(ID3D11Device* a_pDevice, 
		ID3D11DeviceContext* a_pDeviceContext, 
		ID3D11VertexShader* a_pVertexShader,
		ID3D11PixelShader* a_pPixelShader,
		ID3D11InputLayout* a_pInputLayout,
		const std::string& a_sHeightmap,
		int a_nGridWidth,
		int a_nGridDepth)
	{
		TFGrid::Init(a_pDevice, 
			a_pDeviceContext, 
			a_pVertexShader, 
			a_pPixelShader, 
			a_pInputLayout, 
			a_sHeightmap,
			a_nGridWidth, 
			a_nGridDepth);

		m_bUsingHeightmap = false;
		GenerateGrid(a_nGridWidth, a_nGridDepth, 20.0f);


		// Bind normal map
		HR(D3DX11CreateShaderResourceViewFromFile(
			m_pd3dDevice, 
			L"..\\Textures\\water_normal.png", 
			NULL, 
			NULL, 
			&m_pNormalMapSRV, 
			NULL));

		// describe the cb for the WVP matrix
		D3D11_BUFFER_DESC sbd;
		ZeroMemory(&sbd, sizeof(sbd));
		sbd.Usage          = D3D11_USAGE_DEFAULT;
		sbd.ByteWidth      = sizeof(TFBufferPerObjectWater);
		sbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		sbd.CPUAccessFlags = 0;
		sbd.MiscFlags      = 0;

		// Create the constant buffer with the device
		HR(m_pd3dDevice->CreateBuffer(&sbd, NULL, &m_pCBPerObject));
	}

	void TFWaterStill::Update(float a_fDelta)
	{
		m_fWaterOffsetX += a_fDelta * 0.1f;
		m_fWaterOffsetY += -a_fDelta * 0.1f;

		m_matTexTransform = XMMatrixTranslation(fmod(m_fWaterOffsetX, 12.0f), 0, 0);

		m_matTexTransformNeg = XMMatrixTranslation(fmod(m_fWaterOffsetY, 12.0f), 0, 0);
		m_matTexTransformNeg *= XMMatrixScaling(0.5f, 0.5f, 0.5f);
	}

	void TFWaterStill::UpdateResources(const tfMatrix& a_matWVP, 
		const tfMatrix& a_matWorld, 
		const tfMatrix& a_matTexTransform,
		const tfMatrix& a_matTexTransformNeg)
	{
		//UPDATE TRANSFORM RESOURCE
		TFCore::TFBufferPerObjectWater cb;

		// update world matrix
		cb.worldMatrix = XMMatrixTranspose(a_matWorld);

		// Update world inverse transpose matrix (used to transform normals as it will be distorted 
		// with non uniform scaling transforms, see pg. 277 of Luna...
		tfMatrix _wit = a_matWorld;
		_wit.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		tfVector _witDet = XMMatrixDeterminant(_wit);

		cb.worldInvTransposeMatrix = XMMatrixInverse(&_witDet, _wit);

		// update wvp of buffer
		cb.wvpMatrix = XMMatrixTranspose(a_matWVP);

		// update matrix to transform from object to projective texture coords
		cb.TexTransform = XMMatrixTranspose(a_matTexTransform);
		cb.TexTransformNeg = XMMatrixTranspose(a_matTexTransformNeg);

		m_pDeviceContext->UpdateSubresource(m_pCBPerObject , 0, NULL, &cb, 0, 0);
	}

	tfMatrix TFWaterStill::GetTextureTransform() const
	{
		return m_matTexTransform;
	}

	tfMatrix TFWaterStill::GetTextureTransformNeg() const
	{
		return m_matTexTransformNeg;
	}

	void TFWaterStill::BindReflectionMap(ID3D11ShaderResourceView* a_pReflectionMap)
	{
		m_pDeviceContext->PSSetShaderResources(0, 1, &a_pReflectionMap);
	}

	void TFWaterStill::ActivateShaders()
	{
		TFGrid::ActivateShaders();

		// bind normal map for water
		m_pDeviceContext->PSSetShaderResources(1, 1, &m_pNormalMapSRV);
	}

}
