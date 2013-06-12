#pragma once
#include "TFIEffect.h"

namespace TFRendering
{

	class TFFXTerrain : public TFIEffect
	{
	public:
		TFFXTerrain(ID3D11Device* a_pDevice,
		ID3D11DeviceContext* a_pDeviceContext);
		~TFFXTerrain();

		void BatchDraw(const tfMatrix& a_matViewProj, 
			const tfMatrix& a_matLightVPT);
		void SetShadowMap(ID3D11ShaderResourceView* a_pShadowMap);
		void UnbindShadowMap();

	private:
		// only allocate this type on the stack (xmmatrix is 16 byte aligned, causes problems on x86 heap)
		struct BufferPerObject
		{
			tfMatrix   worldMatrix;
			tfMatrix   worldInvTransposeMatrix;
			tfMatrix   wvpMatrix;
			tfMatrix   lightVPT;
		};

	private:
		void UpdateBuffers(const tfFloat4x4& a_matWorld, 
			const tfMatrix& a_matViewProj,
			const tfMatrix& a_matLightVPT);
	};


}