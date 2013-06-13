#include "TFDriver.h"
#include "TFMath.h"
#include "TFInput.h"

using namespace TFCore;

TFApplication::TFApplication()
	:m_pCube1(NULL)
{
	m_pCube1 = new TFCube();
}

TFApplication::~TFApplication()
{
	// Delete renderable objects
	delete m_pCube1;
}

void TFApplication::Init(HINSTANCE hInstance, int nCmdShow)
{
	InitWindowsApp(hInstance, nCmdShow);
	InitD3D();

	// Init shader management
	m_shaderManager.Init(m_pd3dDevice, m_pd3dImmDeviceContext);
	// The next two calls generate vertex and pixel shaders from a stock shader file
	// and set the active vertex and pixel shader objects.
	//m_shaderManager.GenerateDefaultPosColVertexShader();
	//m_shaderManager.GenerateDefaultPosColPixelShader();
	// Generates a constant buffer object that will communicate the WVP matrix between CPU-GPU
	m_shaderManager.GenerateDefaultPosColWVPBuffer();

	// Initialize geometry to pass to shader manager
	m_pCube1->Init(m_pd3dDevice, m_pd3dImmDeviceContext, 1.0f, L"SimplePosCol.hlsl");

	// Init geometry
	//m_cube1.Init(m_pd3dDevice, m_pd3dImmDeviceContext, 1.0f);

	// Set up initial matrices for WVP
	m_matWorld = XMMatrixIdentity();
	m_matProj  = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_nClientWidth / static_cast<float>(m_nClientHeight), 0.01f, 100.0f);
	m_matView  = XMMatrixLookAtLH(XMVectorSet(-5.0f, 0.0f, -5.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void TFApplication::OnResize()
{
	TFWinBase::OnResize();
	// Rebuild projection matrix on window resize
	m_matProj = TFMatrixPerspectiveLH(XM_PIDIV4, m_nClientWidth / static_cast<float>(m_nClientHeight), 0.01f, 100.0f);
}

void TFApplication::Run()
{
	TFCore::TFWinBase::Run();
}

void TFApplication::UpdateScene(float a_fDelta)
{
	TFCore::TFWinBase::UpdateScene(a_fDelta);

	// Update camera (process user input)
	m_fmCamera.Update(a_fDelta);

	// Grab view matrix from the camera
	m_matView = m_fmCamera.GetView();
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

	//cb.world      = TFMatrixTranspose(m_matWorld);
	//cb.view       = TFMatrixTranspose(m_matView);
	//cb.projection = TFMatrixTranspose(m_matProj);

	XMMATRIX _matWVP = XMMatrixMultiply(m_matWorld, m_matView);
	_matWVP = XMMatrixMultiply(_matWVP, m_matProj);
	cb.wvpMatrix = TFMatrixTranspose(_matWVP);


	ID3D11Buffer* _pConstantBuffer = m_shaderManager.GetWVPBuffer();
	m_pd3dImmDeviceContext->UpdateSubresource(_pConstantBuffer, 0, NULL, &cb, 0, 0);
	
	// Render geometry
	//m_pd3dImmDeviceContext->VSSetShader(m_shaderManager.GetActiveVertexShader(), NULL, 0);
	//m_pd3dImmDeviceContext->PSSetShader(m_shaderManager.GetActivePixelShader(), NULL, 0);
	m_pd3dImmDeviceContext->VSSetShader(m_pCube1->GetVertexShader(), NULL, 0);
	m_pd3dImmDeviceContext->PSSetShader(m_pCube1->GetPixelShader(), NULL, 0);
	m_pd3dImmDeviceContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);

	m_pCube1->Draw();

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