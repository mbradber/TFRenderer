#pragma once

#include <d3d11.h>
#include "TFShaderManager.h"
#include "TFTimer.h"

namespace TFCore
{
	class TFWinBase
	{
	public:
		TFWinBase();
		virtual ~TFWinBase();

		LRESULT TFWinBaseProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		virtual bool    InitWindowsApp(HINSTANCE instanceHandle, int show);
		virtual bool    InitD3D();
		virtual void    OnResize();
		virtual void    Run();
		virtual void    UpdateScene(float a_fDelta);
		virtual void    RenderScene();

	protected:
		ID3D11Device*           m_pd3dDevice;
		ID3D11DeviceContext*    m_pd3dImmDeviceContext;
		IDXGISwapChain*         m_pSwapChain;
		ID3D11Debug*            m_pDebug;
		ID3D11Texture2D*        m_pDepthStencilBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11DepthStencilView* m_pDepthStencilView;
		D3D11_VIEWPORT          m_screenViewport;
		D3D_DRIVER_TYPE         m_d3dDriverType;
		UINT                    m_nMsaaQuality;
		bool                    m_bEnable4xMsaa;

		HWND   m_hMainWnd;
		size_t m_nClientWidth;
		size_t m_nClientHeight;
		bool   m_bResizing;

		TFShaderManager m_shaderManager;
		TFTimer m_timer;
	};
}