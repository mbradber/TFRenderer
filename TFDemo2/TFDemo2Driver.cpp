#include "TFDemo2Driver.h"
#include "TFInput.h"
#include "TFUtils.h"
#include <sstream>
#include <string>
#include "TFModelEx.h"
#include "TFFXBlinnPhong.h"
#include "TFFXTerrain.h"
#include "TFTerrainEx.h"
#include "TFFXDepthRender.h"

using namespace TFCore;
using namespace TFRendering;
using namespace std;
using namespace DirectX;

TFDemo2Driver::TFDemo2Driver()
{
}

TFDemo2Driver::~TFDemo2Driver()
{
	// delete FX
	delete m_pBlinnPhongFX;
	delete m_pTerrainFX;
	delete m_pRenderDepthFX;

	// delete models
	delete m_pHouseModel;
	delete m_pTerrain;

	// delete render to texture resources
	delete m_pShadowMapFront;
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
	m_pBlinnPhongFX = new TFFXBlinnPhong(m_pd3dDevice, m_pd3dImmDeviceContext);
	m_pTerrainFX = new TFFXTerrain(m_pd3dDevice, m_pd3dImmDeviceContext);
	m_pRenderDepthFX = new TFFXDepthRender(m_pd3dDevice, m_pd3dImmDeviceContext);

	// create render to texture maps
	m_pShadowMapFront = new TFRendering::TFShadowMap(m_pd3dDevice, m_pd3dImmDeviceContext, 2048, 2048);

	
	// init renderables

	tfMatrix _matWorld = XMMatrixIdentity();

	// HOUSE
	m_pHouseModel = new TFModelEx(m_pd3dDevice, m_pd3dImmDeviceContext, "..\\Models\\house_obj.obj");
	_matWorld  = XMMatrixScaling(0.015f, 0.015f, 0.015f);
	_matWorld *= XMMatrixRotationAxis(m_fmCamera.GetUpVector(), XM_PIDIV2);
	_matWorld *= XMMatrixTranslation(79.0f, 54.5, -70.0f);
	m_pHouseModel->SetWorldMatrix(_matWorld);

	// TERRAIN
	m_pTerrain = new TFTerrainEx(m_pd3dDevice, 
		m_pd3dImmDeviceContext, 
		"..\\Textures\\Terrain\\heightmap_1.bmp", 
		257, 
		257);
	_matWorld = XMMatrixRotationAxis(m_fmCamera.GetUpVector(), -XM_PI);
	m_pTerrain->SetWorldMatrix(_matWorld);
	
	// add renderables to effects
	m_pBlinnPhongFX->AddRenderable(m_pHouseModel);
	m_pTerrainFX->AddRenderable(m_pTerrain);
	m_pRenderDepthFX->AddRenderable(m_pHouseModel);

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
	TFDepthBiasRender(m_pd3dDevice, m_pd3dImmDeviceContext);

	tfMatrix _matWVP;
	tfMatrix _matViewProj = m_lightManager.GetView() * m_lightManager.GetProjection();

	// Set viewport to shadow map viewport, set render target to null (Disables color writes)
	// and set the depth draws to shadow map depth stencil view
	m_pShadowMapFront->SetRenderState();


	m_pRenderDepthFX->BatchDraw(_matViewProj, XMMatrixIdentity());


	// reset render target
	ResetRenderTarget();
}

void TFDemo2Driver::RenderToReflectionMap()
{

}

void TFDemo2Driver::RenderScene()
{
	TFCore::TFWinBase::RenderScene();

	RenderToShadowMap();

	//TFRenderWireframe(m_pd3dDevice, m_pd3dImmDeviceContext);

	// compute view-proj matrix
	tfMatrix _matViewProj = m_matView * m_matProj;

	// draw blinn-phong shaded objects
	m_pBlinnPhongFX->SetShadowMap(m_pShadowMapFront->GetDepthMapSRV());
	m_pBlinnPhongFX->BatchDraw(_matViewProj, m_lightManager.GetVPT());

	// draw terrain objects
	m_pTerrainFX->SetShadowMap(m_pShadowMapFront->GetDepthMapSRV());
	m_pTerrainFX->BatchDraw(_matViewProj, m_lightManager.GetVPT());

	// Display the back buffer (vsync intervals)
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
