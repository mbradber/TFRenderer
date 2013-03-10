#pragma once

#include <d3d11.h>
#include <xnamath.h>

namespace TFCore
{

	class TFLightManager
	{
	public:
		TFLightManager();
		~TFLightManager();

		void Init(ID3D11Device* a_pDevice, ID3D11DeviceContext* a_pDeviceContext);
		void Update(float a_fDeltaTime, const XMFLOAT3& a_vEyePos);

		// Nested directional light structure
		struct TFDirectionalLight
		{
			TFDirectionalLight();

			XMFLOAT4 Ambient;
			XMFLOAT4 Diffuse;
			XMFLOAT4 Specular;
			XMFLOAT3 Direction;
			float    _pad;
		};

		TFDirectionalLight& GetActiveDirectionalLight() const;

	private:

		// Buffer for directional light object
		struct TFBufferDirectionalLight
		{
			TFDirectionalLight DirLight;
			XMFLOAT3 EyePos;
			float    _pad;
		};

		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		//TODO: Should probably group these in a pair or struct (the light and the struct that maps its buffer)
		TFDirectionalLight m_directionalLight1;
		TFBufferDirectionalLight m_directionaLight1Buffer;
		ID3D11Buffer* m_pCBDirectionalLight;
	};

}

