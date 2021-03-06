
#include "TFCore.h"
#include "TFUtils.h"
#include "TFInput.h"
#include <sstream>

namespace TFCore
{
	/*** Handle to the application to pass to the windows procedure ***/
	TFWinBase* g_TFWinBase = NULL;

	/*** Global callback function which will forward messages to application class ***/
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		TF_ASSERT(g_TFWinBase != NULL, FILE_NAME, LINE_NO);
		return g_TFWinBase->TFWinBaseProc(hWnd, msg, wParam, lParam);
	}

	/*** Ctor ***/
	TFWinBase::TFWinBase()
		:m_pd3dDevice(NULL),
		 m_pd3dImmDeviceContext(NULL),
		 m_pSwapChain(NULL),
		 m_pDebug(NULL),
		 m_pDepthStencilBuffer(NULL),
		 m_pRenderTargetView(NULL),
		 m_pDepthStencilView(NULL),
		 m_d3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
		 m_nMsaaQuality(0),
		 m_bEnable4xMsaa(false),
		 m_nClientWidth(1024),
		 m_nClientHeight(768),
		 m_bResizing(false)
	{
		g_TFWinBase = this;
	}

	/*** Dtor. Release all COM references ***/
	TFWinBase::~TFWinBase()
	{
		g_TFWinBase = NULL;

		if(m_pd3dImmDeviceContext)
		{
			m_pd3dImmDeviceContext->ClearState();
		}

		ReleaseCOM(m_pd3dDevice);
		ReleaseCOM(m_pd3dImmDeviceContext);
		ReleaseCOM(m_pSwapChain);
		ReleaseCOM(m_pDebug);
		ReleaseCOM(m_pDepthStencilBuffer);
		ReleaseCOM(m_pRenderTargetView);
		ReleaseCOM(m_pDepthStencilView);

		ReleaseCOM(m_pRSWireframe);
		ReleaseCOM(m_pRSNoCull);
		ReleaseCOM(m_pRSFrontFaceCull);
		ReleaseCOM(m_pRSDepthBias);
		ReleaseCOM(m_pDSLessEqual);
	}

	/*** Windows callback (handle windows messages here ***/
	LRESULT TFWinBase::TFWinBaseProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch(msg)
		{
		case WM_SIZE:
			m_nClientWidth  = LOWORD(lParam);
			m_nClientHeight = HIWORD(lParam);
			if(m_pd3dDevice)
			{
				if(wParam == SIZE_MAXIMIZED)
				{
					OnResize();
				}
				else if(wParam == SIZE_RESTORED)
				{
					if(!m_bResizing)
					{
						OnResize();
					}
				}
			}
			return 0;
		case WM_ENTERSIZEMOVE:
			m_bResizing = true;
			return 0;
		case WM_EXITSIZEMOVE:
			m_bResizing = false;
			OnResize();
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	/*** Init window, configure main window properties here ***/
	bool TFWinBase::InitWindowsApp(HINSTANCE instanceHandle, int show)
	{
		WNDCLASS wc;

		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = WndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = instanceHandle;
		wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor       = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = 0;
		wc.lpszClassName = L"BasicWndClass";

		if(!RegisterClass(&wc))
		{
			MessageBox(0, L"RegisterClass FAILED", 0, 0);
			return false;
		}

		m_hMainWnd = CreateWindow(
			L"BasicWndClass",
			L"TreeFrog Engine 1.0",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			m_nClientWidth,
			m_nClientHeight,
			0,
			0,
			instanceHandle,
			0
			);

		if(m_hMainWnd == 0)
		{
			MessageBox(0, L"CreateWindow FAILED", 0, 0);
			return false;
		}

		ShowWindow(m_hMainWnd, show);
		UpdateWindow(m_hMainWnd);

		// Init input
		TFInput::Instance()->InitInput(instanceHandle, m_hMainWnd);

		return true;
	}

	/*** Initialize Direct3D. Use this method to query feature levels, MSAA and configure 
		 the swap chain. ***/
	bool TFWinBase::InitD3D()
	{
		UINT _createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		_createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL _featureLevel;
		HRESULT _hr = D3D11CreateDevice(
			0,
			m_d3dDriverType,
			0,
			_createDeviceFlags,
			0,0,
			D3D11_SDK_VERSION,
			&m_pd3dDevice,
			&_featureLevel,
			&m_pd3dImmDeviceContext
			);

		if(FAILED(_hr))
		{
			MessageBox(0, L"D3D11CreateDevice Failed", 0, 0);
			return false;
		}

		if(_featureLevel != D3D_FEATURE_LEVEL_11_0)
		{
			MessageBox(0, L"Direct3D feature level 11 unsupported.", 0, 0);
			return false;
		}

#if defined (DEBUG) || defined(_DEBUG)
		_hr = m_pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&m_pDebug);

		if(FAILED(_hr))
		{
			MessageBox(0, L"Failed to acquire debug object.", 0, 0);
			return false;
		}
#endif

		_hr = m_pd3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8X8_UNORM, 4, &m_nMsaaQuality);

		if(FAILED(_hr))
		{
			MessageBox(0, L"Sample check failed.", 0, 0);
			return false;
		}

		TF_ASSERT(m_nMsaaQuality > 0, FILE_NAME, LINE_NO);

		DXGI_SWAP_CHAIN_DESC sd;
		sd.BufferDesc.Width  = m_nClientWidth;
		sd.BufferDesc.Height = m_nClientHeight;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		if( m_bEnable4xMsaa )
		{
			sd.SampleDesc.Count   = 4;
			sd.SampleDesc.Quality = m_nMsaaQuality-1;
		}
		else
		{
			sd.SampleDesc.Count   = 1;
			sd.SampleDesc.Quality = 0;
		}

		sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount  = 1;
		sd.OutputWindow = m_hMainWnd;
		sd.Windowed     = true;
		sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags        = 0;


		IDXGIDevice* dxgiDevice = 0;
		HR(m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

		IDXGIAdapter* dxgiAdapter = 0;
		HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

		IDXGIFactory* dxgiFactory = 0;
		HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

		HR(dxgiFactory->CreateSwapChain(m_pd3dDevice, &sd, &m_pSwapChain));

		ReleaseCOM(dxgiDevice);
		ReleaseCOM(dxgiAdapter);
		ReleaseCOM(dxgiFactory);

		OnResize();

		/*** Init common application states ***/

		D3D11_RASTERIZER_DESC rd;

		// Create wireframe/no cull mode for debug
		ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));
		rd.FillMode = D3D11_FILL_WIREFRAME;
		rd.CullMode = D3D11_CULL_NONE;
		rd.FrontCounterClockwise = false;
		rd.DepthClipEnable = true;

		m_pd3dDevice->CreateRasterizerState(&rd, &m_pRSWireframe);

		// Create no cull render mode
		ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_NONE;
		rd.FrontCounterClockwise = false;
		rd.DepthClipEnable = true;

		m_pd3dDevice->CreateRasterizerState(&rd, &m_pRSNoCull);

		// Create front face cull render mode
		ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_FRONT;

		m_pd3dDevice->CreateRasterizerState(&rd, &m_pRSFrontFaceCull);

		// Create depth bias render mode
		ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));
		rd.DepthBias = 8000;
		rd.DepthBiasClamp = 0.0f;
		rd.SlopeScaledDepthBias = 1.0f;
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_BACK;

		m_pd3dDevice->CreateRasterizerState(&rd, &m_pRSDepthBias);

		// Create depth less than or equal comparison state
		D3D11_DEPTH_STENCIL_DESC _dsDesc;
		ZeroMemory(&_dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

		_dsDesc.DepthEnable = true;
		_dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		_dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		_dsDesc.StencilEnable = false;

		m_pd3dDevice->CreateDepthStencilState(&_dsDesc, &m_pDSLessEqual);

		return true;
	}

	/*** Encapsulates all necessary adjustments to the application upon a window resize message.
		 Here the swap chain is resized based on the new window dimensions, then a render target view
		 and depth stencil view is reconstructed with those new dimensions. The render viewport is
		 also adjusted to reflect the changes ***/
	void TFWinBase::OnResize()
	{
		TF_ASSERT(m_pd3dImmDeviceContext != NULL, FILE_NAME, LINE_NO);
		TF_ASSERT(m_pd3dDevice != NULL, FILE_NAME, LINE_NO);
		TF_ASSERT(m_pSwapChain != NULL, FILE_NAME, LINE_NO);

		ReleaseCOM(m_pRenderTargetView);
		ReleaseCOM(m_pDepthStencilView);
		ReleaseCOM(m_pDepthStencilBuffer);

		HR(m_pSwapChain->ResizeBuffers(1, m_nClientWidth, m_nClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
		ID3D11Texture2D* backBuffer;
		HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
		HR(m_pd3dDevice->CreateRenderTargetView(backBuffer, 0, &m_pRenderTargetView));
		ReleaseCOM(backBuffer);

		D3D11_TEXTURE2D_DESC depthStencilDesc;

		depthStencilDesc.Width     = m_nClientWidth;
		depthStencilDesc.Height    = m_nClientHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;

		if( m_bEnable4xMsaa )
		{
			depthStencilDesc.SampleDesc.Count   = 4;
			depthStencilDesc.SampleDesc.Quality = m_nMsaaQuality-1;
		}
		else
		{
			depthStencilDesc.SampleDesc.Count   = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
		}

		depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0; 
		depthStencilDesc.MiscFlags      = 0;

		HR(m_pd3dDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer));
		HR(m_pd3dDevice->CreateDepthStencilView(m_pDepthStencilBuffer, 0, &m_pDepthStencilView));

		m_pd3dImmDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		m_screenViewport.TopLeftX = 0;
		m_screenViewport.TopLeftY = 0;
		m_screenViewport.Width    = static_cast<float>(m_nClientWidth);
		m_screenViewport.Height   = static_cast<float>(m_nClientHeight);
		m_screenViewport.MinDepth = 0.0f;
		m_screenViewport.MaxDepth = 1.0f;

		m_pd3dImmDeviceContext->RSSetViewports(1, &m_screenViewport);
	}

	/*** Quick routine to reset the render target to the Texture2D that was created for the back buffer
		 and the depth stencil view. Also resets the application viewport. This is primary used after 
		 the application has used render to texture for shadow mapping and reflection to texture. ***/
	void TFWinBase::ResetRenderTarget()
	{
		// reset render state to original
		m_pd3dImmDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
		m_pd3dImmDeviceContext->RSSetViewports(1, &m_screenViewport);
	}

	/*** Game loop. Process messages then update/render ***/
	void TFWinBase::Run()
	{
		MSG msg = {0};

		while(msg.message != WM_QUIT)
		{
			if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if(msg.message == WM_QUIT)
				{
					break;
				}
			}
			else
			{
				m_timer.UpdateTimer();
				float _fElapsedTime = m_timer.GetElapsedTime();

				UpdateScene(_fElapsedTime);
				RenderScene();
			}
		}
	}

	/*** Put any common application update code here. ***/
	void TFWinBase::UpdateScene(float a_fDelta)
	{
		//std::stringstream _ss;
		//_ss << "TreeFrog Engine 1.1" << " MPF:" << a_fDelta * 1000.0f;
		//SetWindowTextA(m_hMainWnd, _ss.str().c_str());
	}

	/*** Put any common application render code here. Currently this simply clears the render
		 target view and depth-stencil view ***/
	void TFWinBase::RenderScene()
	{
		// Just clear the backbuffer and depth stencil view
		float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
		m_pd3dImmDeviceContext->ClearRenderTargetView( m_pRenderTargetView, ClearColor );
		m_pd3dImmDeviceContext->ClearDepthStencilView( m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

}