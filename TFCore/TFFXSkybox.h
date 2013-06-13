#pragma once
#include "TFIEffect.h"
#include <DirectXMath.h>

namespace TFRendering
{
	class TFFXSkybox : public TFIEffect
	{
	public:
		TFFXSkybox(ID3D11Device* a_pDevice,
			ID3D11DeviceContext* a_pDeviceContext);
		~TFFXSkybox();

		void BatchDraw(const tfMatrix& a_matViewProj,
			const tfMatrix& a_matLightVPT);

	private:
		struct BufferPerObject
		{
			tfMatrix   wvpMatrix;
		};

	private:
		void UpdateBuffers(const tfFloat4x4& a_matWorld, 
			const tfMatrix& a_matViewProj,
			const tfMatrix& a_matLightVPT);

		ID3D11ShaderResourceView* m_cubeMapSRV;
	};

}