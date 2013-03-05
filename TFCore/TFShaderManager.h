#pragma once

#include <d3d11.h>
#include <vector>
#include "TFTypes.h"
#include "TFIRenderable.h"

namespace TFCore
{
	// Layout of the buffer for world-view-projection matrix
	struct TFBufferWVP
	{
		XMMATRIX wvpMatrix;
	};

	class TFShaderManager
	{
	public:
		TFShaderManager();
		~TFShaderManager();

		void Init(ID3D11Device* a_pd3dDevice, ID3D11DeviceContext* a_pDeviceContext);
		void CompileShaderFromFile(WCHAR* a_cbFileName, LPCSTR a_pEntryPoint, LPCSTR a_pShaderModel, ID3DBlob** a_ppBlobOut);
		void RegisterRenderable(TFIRenderable* a_pRenderable);
		void GenerateDefaultPosColVertexShader();
		void GenerateDefaultPosColPixelShader();
		void GenerateDefaultPosColWVPBuffer();
		inline ID3D11Buffer* GetWVPBuffer() { return m_pConstantBuffer; }
		inline ID3D11VertexShader* GetActiveVertexShader() { return m_pVertexShader; }
		inline ID3D11PixelShader* GetActivePixelShader()  { return m_pPixelShader;  }

	private:
		ID3D11Device*				m_pd3dDevice;
		ID3D11DeviceContext*		m_pDeviceContext;
		ID3D11VertexShader*			m_pVertexShader;
		ID3D11PixelShader*			m_pPixelShader;
		ID3D11InputLayout*			m_pInputLayout;
		ID3D11Buffer*				m_pConstantBuffer;
		WCHAR*						m_cbDefaultPosColShaderPath;
		std::vector<TFIRenderable*> m_pRenderables;
	};

}

