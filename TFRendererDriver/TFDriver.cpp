#include "TFDriver.h"
#include "TFMath.h"

void TFApplication::Init(HINSTANCE hInstance, int nCmdShow)
{
	InitWindowsApp(hInstance, nCmdShow);
	InitD3D();

	// Init shader management
	m_shaderManager.Init(m_pd3dDevice, m_pd3dImmDeviceContext);
	// The next two calls generate vertex and pixel shaders from a stock shader file
	// and set the active vertex and pixel shader objects.
	m_shaderManager.GenerateDefaultPosColVertexShader();
	m_shaderManager.GenerateDefaultPosColPixelShader();
	// Generates a constant buffer object that will communicate the WVP matrix between CPU-GPU
	m_shaderManager.GenerateDefaultPosColWVPBuffer();

	// Init geometry
	m_cube1.Init(m_pd3dDevice, m_pd3dImmDeviceContext, 1.0f);

	// Set up initial matrices for WVP
	m_matWorld = TFGetMatrixIdentity();
	m_matProj = TFMatrixPerspectiveLH(TF_PIDIV2, m_nClientWidth / static_cast<float>(m_nClientHeight), 0.01f, 100.0f);
}

void TFApplication::Run()
{
	TFCore::TFWinBase::Run();
}

void TFApplication::UpdateScene(float a_fDelta)
{
	TFCore::TFWinBase::UpdateScene(a_fDelta);

	tfVector eye = TFVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	tfVector at  = TFVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	tfVector up  = TFVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_matView = TFMatrixLookAtLH(eye, at, up);
}

void TFApplication::RenderScene()
{
	TFCore::TFWinBase::RenderScene();

	// Set wireframe/no cull mode for debug
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = false;
	rd.DepthClipEnable = true;

	ID3D11RasterizerState* _pRasterizerState;
	m_pd3dDevice->CreateRasterizerState(&rd, &_pRasterizerState);
	m_pd3dImmDeviceContext->RSSetState(_pRasterizerState);

	// Update WVP constant buffer
	TFCore::TFBufferWVP cb;

	cb.world      = TFMatrixTranspose(m_matWorld);
	cb.view       = TFMatrixTranspose(m_matView);
	cb.projection = TFMatrixTranspose(m_matProj);

	m_pd3dImmDeviceContext->UpdateSubresource(m_shaderManager.GetWVPBuffer(), 0, NULL, &cb, 0, 0);
	ID3D11Buffer* _pConstantBuffer = m_shaderManager.GetWVPBuffer();

	// Render geometry
	m_pd3dImmDeviceContext->VSSetShader(m_shaderManager.GetActiveVertexShader(), NULL, 0);
	m_pd3dImmDeviceContext->PSSetShader(m_shaderManager.GetActivePixelShader(), NULL, 0);
	m_pd3dImmDeviceContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);

	m_cube1.Draw();

	m_pSwapChain->Present( 0, 0 );
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, int nCmdShow)
{
	TFApplication _tfApp;
	_tfApp.Init(hInstance, nCmdShow);
	_tfApp.Run();

	return 0;
}