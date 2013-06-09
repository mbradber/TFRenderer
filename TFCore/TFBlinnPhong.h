#pragma once

#include "TFIEffect.h"
#include "TFTypes.h"

namespace TFRendering
{

	class TFBlinnPhong : public TFIEffect
	{
	public:
		TFBlinnPhong(ID3D11Device* a_pDevice,
			ID3D11DeviceContext* a_pDeviceContext);
		~TFBlinnPhong();

		void BatchDraw(const tfMatrix& a_matViewProj, 
			const tfMatrix& a_matLightVPT);

	private:
		// disable copying
		TFBlinnPhong(TFBlinnPhong&);
		TFBlinnPhong& operator=(TFBlinnPhong&);

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
			const tfMatrix& a_matLightVP);
	};

}