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
	}

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
		 Direction(0.57735f, -0.57735f, 0.57735f)
	{
	}

}
