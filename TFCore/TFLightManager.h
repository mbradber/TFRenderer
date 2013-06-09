#pragma once

#include <d3d11.h>
#include "TFTypes.h"

namespace TFCore
{

	class TFLightManager
	{
	public:
		TFLightManager();
		~TFLightManager();

		void Init(ID3D11Device* a_pDevice, ID3D11DeviceContext* a_pDeviceContext);
		void Update(float a_fDeltaTime, const tfFloat3& a_vEyePos, bool a_bInitialTick);

		// Nested directional light structure
		struct TFDirectionalLight
		{
			TFDirectionalLight();

			tfFloat4 Ambient;
			tfFloat4 Diffuse;
			tfFloat4 Specular;
			tfFloat3 Direction;
			float    _pad;
		};

		TFDirectionalLight& GetActiveDirectionalLight() const;
		tfMatrix GetView();
		tfMatrix GetProjection();
		tfMatrix GetVPT();
		tfFloat3 GetPosition() const;

	private:

		// Buffer for directional light object
		struct TFBufferDirectionalLight
		{
			TFDirectionalLight DirLight;
			tfFloat3 EyePos;
			float    _pad;
		};

		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		//TODO: Should probably group these in a pair or struct (the light and the struct that maps its buffer)
		TFDirectionalLight m_directionalLight1;
		TFBufferDirectionalLight m_directionaLight1Buffer;
		ID3D11Buffer* m_pCBDirectionalLight;
		tfVector m_vPos; // TODO: dont use tfVector as class member
		float m_fLightDistance;
	};

}

