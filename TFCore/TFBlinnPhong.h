#pragma once
#include "TFEffect.h"
#include <xnamath.h>

namespace TFRendering
{

	class TFBlinnPhong : public TFEffect
	{
	public:
		TFBlinnPhong();
		~TFBlinnPhong();

		void Init();
		void BatchDraw(const XMMATRIX& a_matViewProj, const XMMATRIX& a_matLightVPT);

	private:

		struct BufferPerObject
		{
			XMMATRIX   worldMatrix;
			XMMATRIX   worldInvTransposeMatrix;
			XMMATRIX   wvpMatrix;
			XMMATRIX   lightVPT;
		};

	private:
		void UpdateBuffers(const XMMATRIX& a_matWorld, 
			const XMMATRIX& a_matViewProj,
			const XMMATRIX& a_matLightVP);
	};

}