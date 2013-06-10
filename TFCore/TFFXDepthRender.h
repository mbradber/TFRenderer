#pragma once
#include "TFIEffect.h"

namespace TFRendering
{

	class TFFXDepthRender : public TFIEffect
	{
	public:
		TFFXDepthRender(ID3D11Device* a_pDevice,
			ID3D11DeviceContext* a_pDeviceContext);
		~TFFXDepthRender();
		
		void BatchDraw(const tfMatrix& a_matViewProj, 
			const tfMatrix& a_matLightVPT);

	private:
		// disable copying
		TFFXDepthRender(TFFXDepthRender&);
		TFFXDepthRender& operator=(TFFXDepthRender&);

	private:

		// only allocate this type on the stack (xmmatrix is 16 byte aligned, causes problems on x86 heap)
		struct BufferPerObject
		{
			tfMatrix   wvpMatrix;
		};

	private:
		void UpdateBuffers(const tfFloat4x4& a_matWorld, 
			const tfMatrix& a_matViewProj);
	};

}