#include "TFLightManager.h"
#include "TFUtils.h"

namespace TFCore
{

	TFLightManager::TFLightManager()
		:m_pDevice(NULL),
		 m_pDeviceContext(NULL)
	{

	}


	TFLightManager::~TFLightManager()
	{
		ReleaseCOM(m_pCBDirectionalLight);
	}

	void TFLightManager::Init(ID3D11Device* a_pDevice, ID3D11DeviceContext* a_pDeviceContext)
	{
		// Assign members
		m_pDevice = a_pDevice;
		m_pDeviceContext = a_pDeviceContext;

		// Create shader resources to be bound to pipeline

		// describe the cb for the directional light
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage          = D3D11_USAGE_DEFAULT;
		bd.ByteWidth      = sizeof(TFBufferDirectionalLight);
		bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags      = 0;

		// Create the constant buffer with the device
		HR(m_pDevice->CreateBuffer(&bd, NULL, &m_pCBDirectionalLight));

		m_pDeviceContext->UpdateSubresource(m_pCBDirectionalLight, 0, NULL, &m_directionaLight1Buffer, 0, 0);

		// Bind the constant buffer containing the light info to the shaders it will be used in
		m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pCBDirectionalLight);
	}

	// TODO: Should probably move the updating of the light away from here, its kind of weird that
	// this file is externally assuming the constant buffer for the light will be at slot 1
	void TFLightManager::Update(float a_fDeltaTime, const XMFLOAT3& a_vEyePos)
	{
		// Rotate the light over time
		static float _rotationX = 0.0f;
		static float _rotationZ = 0.0f;

		_rotationX += a_fDeltaTime;
		_rotationZ += a_fDeltaTime;
		
		m_directionalLight1.Direction.x = cosf(_rotationX);
		m_directionalLight1.Direction.z = sinf(_rotationZ);

		// UPDATE LIGHT RESOURCE
		m_directionaLight1Buffer.DirLight = m_directionalLight1;
		m_directionaLight1Buffer.EyePos   = a_vEyePos;

		m_pDeviceContext->UpdateSubresource(m_pCBDirectionalLight, 0, NULL, &m_directionaLight1Buffer, 0, 0);

		// Bind the constant buffer containing the light info to the shaders it will be used in
		m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pCBDirectionalLight);
	}

	TFLightManager::TFDirectionalLight::TFDirectionalLight()
		:Ambient(0.3f, 0.3f, 0.3f, 1.0f),
		 Diffuse(0.8f, 0.8f, 0.8f, 1.0f),
		 Specular(0.5f, 0.5f, 0.5f, 1.0f),
		 Direction(0.577350269f, -0.577350269f, 0.577350269f)
	{
	}

	// TODO: Do a more formal calculation of the estimated position of the primary light source (using a scene
	// bounding sphere instead of some arbitrary number

	// TODO: Look into the "At" vector calculation, the Luna book says it should be the center of the scene, not the
	// position + forward vector
	XMMATRIX TFLightManager::GetView()
	{
		XMVECTOR _vDir = XMVectorSet(m_directionalLight1.Direction.x, m_directionalLight1.Direction.y, m_directionalLight1.Direction.z, 0.0f);
		XMVector4Normalize(_vDir);
		XMVECTOR _vPos = -_vDir * 40;
		//XMVECTOR _vAt  = _vPos + _vDir;
		XMVECTOR _vAt = XMVectorZero();
		XMVECTOR _vUp  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		return XMMatrixLookAtLH(_vPos, _vAt, _vUp);
	}

	// TODO: Do a more formal calculation of the dimensions of this orthographic viewing volume
	// TODO: Don't query this every frame
	XMMATRIX TFLightManager::GetProjection()
	{
		return XMMatrixOrthographicOffCenterLH(-60, 60, -35, 35, -20.0f, 100.0f);
		//return XMMatrixPerspectiveFovLH(XM_PIDIV4, 1008.f / 730.f,  50, 100.0f);
	}

	XMMATRIX TFLightManager::GetVPT()
	{
		// Transform NDC space to texture space (Taken from Luna DX 11 book).
		XMMATRIX T( 
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f);


		return GetView() * GetProjection() * T;

		//return GetView() * GetProjection();
	}

}
