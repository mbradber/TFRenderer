#include "TFLightManager.h"
#include "TFUtils.h"

namespace TFCore
{
	using namespace DirectX;

	TFLightManager::TFLightManager()
		:m_pDevice(NULL),
		 m_pDeviceContext(NULL)
	{
		//m_vPos = XMVectorSet(-146.0f, 141.0f, 62.0f, 0.0f);
		m_vPos = XMVectorSet(-111, 99, 48, 0);
		tfVector _vLightDistance = XMVector4Length(m_vPos);
		tfFloat4 _f4LightDistance;
		XMStoreFloat4(&_f4LightDistance, _vLightDistance);

		m_fLightDistance = _f4LightDistance.x;
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
	void TFLightManager::Update(float a_fDeltaTime, const tfFloat3& a_vEyePos, bool a_bInitialTick)
	{
		// Rotate the light over time
		static float _rotationX = 0;
		static float _rotationZ = 0;

		if(!a_bInitialTick)
		{
			_rotationX += a_fDeltaTime / 5.0f;
			_rotationZ += a_fDeltaTime / 5.0f;

			tfFloat4 _f4NewPos;
			XMStoreFloat4(&_f4NewPos, m_vPos);

			_f4NewPos.x = cosf(_rotationX) * m_fLightDistance;
			_f4NewPos.z = sinf(_rotationZ) * m_fLightDistance;

			m_vPos = XMLoadFloat4(&_f4NewPos);
		}

		tfVector _vLightDir = XMVectorZero() - m_vPos;
		_vLightDir = XMVector4Normalize(_vLightDir);
		tfFloat4 _f4LightDir;
		XMStoreFloat4(&_f4LightDir, _vLightDir);
		m_directionalLight1.Direction.x = _f4LightDir.x;
		m_directionalLight1.Direction.y = _f4LightDir.y;
		m_directionalLight1.Direction.z = _f4LightDir.z;

		// UPDATE LIGHT RESOURCE
		m_directionaLight1Buffer.DirLight = m_directionalLight1;
		m_directionaLight1Buffer.EyePos   = a_vEyePos;

		m_pDeviceContext->UpdateSubresource(m_pCBDirectionalLight, 0, NULL, &m_directionaLight1Buffer, 0, 0);

		// Bind the constant buffer containing the light info to the shaders it will be used in
		m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pCBDirectionalLight);
	}

	tfFloat3 TFLightManager::GetPosition() const
	{
		tfFloat3 _vPos;
		XMStoreFloat3(&_vPos, m_vPos);
		
		return _vPos;
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
	tfMatrix TFLightManager::GetView()
	{
		//tfVector _vAt  = _vPos + _vDir;
		tfVector _vAt = XMVectorZero();
		tfVector _vUp  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		return XMMatrixLookAtLH(m_vPos, _vAt, _vUp);
	}

	// TODO: Do a more formal calculation of the dimensions of this orthographic viewing volume
	// TODO: Don't query this every frame
	tfMatrix TFLightManager::GetProjection()
	{
		return XMMatrixOrthographicOffCenterLH(-140, 140, -70, 150, 0, 250);
		//return XMMatrixPerspectiveFovLH(XM_PIDIV4, 1008.f / 730.f,  0, 1000.0f);
	}

	tfMatrix TFLightManager::GetVPT()
	{
		// Transform NDC space to texture space (Taken from Luna DX 11 book).
		tfMatrix T( 
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f);


		return GetView() * GetProjection() * T;

		//return GetView() * GetProjection();
	}

}
