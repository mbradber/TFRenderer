#include "TFDemo2Driver.h"
#include "TFInput.h"
#include "TFUtils.h"
#include <sstream>
#include <string>
#include "TFModelEx.h"

using namespace TFCore;
using namespace TFRendering;
using namespace std;
using namespace DirectX;

TFDemo2Driver::TFDemo2Driver()
{
}

TFDemo2Driver::~TFDemo2Driver()
{
	delete m_pBlinnPhongFX;
	delete m_pTerrainFX;

	delete m_pHouseModel;
	delete m_pTerrain;
}

void TFDemo2Driver::Init(HINSTANCE hInstance, int a_nCmdShow)
{
	InitWindowsApp(hInstance, a_nCmdShow);
	InitD3D();

	// init components
	m_lightManager.Init(m_pd3dDevice, m_pd3dImmDeviceContext);
	m_shaderManager.Init(m_pd3dDevice);

	// bind samplers
	TFIEffect::InitializeSamplers(m_pd3dDevice, m_pd3dImmDeviceContext);

	// build effects from shaders
	m_pBlinnPhongFX = new TFBlinnPhong(m_pd3dDevice, m_pd3dImmDeviceContext);


	tfMatrix _matWorld = XMMatrixIdentity();
	
	// init renderables
	m_pHouseModel = new TFModelEx(m_pd3dDevice, m_pd3dImmDeviceContext, "..\\Models\\house_obj.obj");
	_matWorld  = XMMatrixScaling(0.015f, 0.015f, 0.015f);
	m_pHouseModel->SetWorldMatrix(_matWorld);
	
	// add renderables to effects
	m_pBlinnPhongFX->AddRenderable(m_pHouseModel);

	// Set up initial matrices for WVP
	m_matWorld = XMMatrixIdentity();
	m_matProj  = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_nClientWidth / static_cast<float>(m_nClientHeight), 1.0f, 1000.0f);
	m_matView  = XMMatrixLookAtLH(XMVectorSet(-5.0f, 0.0f, -5.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	// Update the lights
	m_lightManager.Update(1.0f, m_fmCamera.GetPosition(), true);
}

void TFDemo2Driver::OnResize()
{
	TFWinBase::OnResize();
	// Rebuild projection matrix on window resize
	m_matProj = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_nClientWidth / static_cast<float>(m_nClientHeight), 1.0f, 1000.0f);
}

void TFDemo2Driver::UpdateScene(float a_fDelta)
{
	TFCore::TFWinBase::UpdateScene(a_fDelta);
	TFInput::Instance()->DetectInput();

	// Update camera (process user input)
	m_fmCamera.Update(a_fDelta);

	// Grab view matrix from the camera
	m_matView = m_fmCamera.GetView();
}

void TFDemo2Driver::RenderToShadowMap()
{

}

void TFDemo2Driver::RenderToReflectionMap()
{

}

void TFDemo2Driver::RenderScene()
{
	TFCore::TFWinBase::RenderScene();

	//TFRenderWireframe(m_pd3dDevice, m_pd3dImmDeviceContext);

	// compute view-proj matrix
	tfMatrix _matViewProj = m_matView * m_matProj;

	m_pBlinnPhongFX->BatchDraw(_matViewProj, m_lightManager.GetVPT());

	// Display the back buffer (vsync intervals)
	//m_pSwapChain->Present( 1, 0 );
	m_pSwapChain->Present(0, 0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, int nCmdShow)
{
	TFDemo2Driver _tfApp;
	_tfApp.Init(hInstance, nCmdShow);
	_tfApp.Run();

	return 0;
}
