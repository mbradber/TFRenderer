#pragma once
#include "TFIEffect.h"
#include <DirectXMath.h>

namespace TFRendering
{
	class TFFXWaterStill : public TFIEffect
	{
	public:
		TFFXWaterStill(ID3D11Device* a_pDevice,
			ID3D11DeviceContext* a_pDeviceContext);
		~TFFXWaterStill();

		void BatchDraw(const tfMatrix& a_matViewProj, 
			const tfMatrix& a_matLightVPT);

		void SetReflectionMap(ID3D11ShaderResourceView* a_pReflectionMap);
		void UnbindReflectionMap();
		void SetBlendState();

		void UpdateWater(float a_fDelta);

	private:
		// only allocate this type on the stack (xmmatrix is 16 byte aligned, causes problems on x86 heap)
		struct BufferPerObject
		{

			tfMatrix   worldMatrix;
			tfMatrix   worldInvTransposeMatrix;
			tfMatrix   wvpMatrix;
			tfMatrix   texTransformPos;
			tfMatrix   texTransformNeg;
		};

	private:
		// this method needs to animate the water as well
		void UpdateBuffers(const tfFloat4x4& a_matWorld, 
			const tfMatrix& a_matViewProj,
			const tfMatrix& a_matLightVPT);

		float m_fWaterOffsetX;
		float m_fWaterOffsetY;
		ID3D11BlendState* m_pBlendState1;
	};

}