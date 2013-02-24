#pragma once

#include <d3d11.h>
#include "TFTypes.h"

namespace TFCore
{
	// Layout of the buffer for world-view-projection matrix
	struct TFBufferWVP
	{
		tfMatrix world;
		tfMatrix view;
		tfMatrix projection;
	};

	class TFShaderManager
	{
	public:
		TFShaderManager(void);
		~TFShaderManager(void);

		void Init(ID3D11Device* a_pd3dDevice);
		void CompileShaderFromFile(WCHAR* a_cbFileName, LPCSTR a_pEntryPoint, LPCSTR a_pShaderModel, ID3DBlob** a_ppBlobOut);
		void GenerateDefaultPosColVertexShader();
		void GenerateDefaultPosColPixelShader();
		void GenerateDefaultPosColWVPBuffer();
		inline ID3D11Buffer* GetWVPBuffer() { return m_pConstantBuffer; }
		inline ID3D11VertexShader* GetActiveVertexShader() { return m_pVertexShader; }
		inline ID3D11PixelShader* GetActivePixelShader()  { return m_pPixelShader;  }

	private:
		ID3D11Device*        m_pd3dDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		ID3D11VertexShader*  m_pVertexShader;
		ID3D11PixelShader*   m_pPixelShader;
		ID3D11InputLayout*   m_pInputLayout;
		ID3D11Buffer*        m_pConstantBuffer;
	};

}

