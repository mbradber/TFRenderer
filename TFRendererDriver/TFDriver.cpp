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

	m_pCube1->Init(m_pd3dDevice, m_pd3dImmDeviceContext, 1.0f, L"SimpleDirLight.hlsl");

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
	//D3D11_RASTERIZER_DESC rd;
	//ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));
	//rd.FillMode = D3D11_FILL_WIREFRAME;
	//rd.CullMode = D3D11_CULL_NONE;
	//rd.FrontCounterClockwise = false;
	//rd.DepthClipEnable = true;

	//ID3D11RasterizerState* _pRasterizerState;
	//m_pd3dDevice->CreateRasterizerState(&rd, &_pRasterizerState);
	//m_pd3dImmDeviceContext->RSSetState(_pRasterizerState);

	// Update the geometry with their respective transforms
	XMMATRIX _matWVP = m_matWorld * m_matView * m_matProj;

	// Update and activate the shaders, then draw the geometry
	m_pCube1->UpdateResources(_matWVP, m_matWorld, m_fmCamera.GetPosition());
	m_pCube1->ActivateShaders();
	m_pCube1->Draw();

	// Display the back buffer
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