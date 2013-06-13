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
#include "TFWaterStillEx.h"
#include "TFFXWaterStill.h"
#include "TFFXSkybox.h"
#include "TFFXFoliage.h"

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
	delete m_pWaterStillFX;
	delete m_pSkyboxFX;
	delete m_pFoliageFX;

	// delete models
	delete m_pHouseModel;
	delete m_pTree1;
	delete m_pTree2;
	delete m_pTree3;
	delete m_pTerrain;
	delete m_pWater1;
	delete m_pWater2;
	delete m_pSkybox;
	delete m_pGnome;
	delete m_pDoor1;

	// delete render to texture resources
	delete m_pShadowMapFront;
	delete m_pReflectionMap;
}

void TFDemo2Driver::Init(HINSTANCE hInstance, int a_nCmdShow)
{
	InitWindowsApp(hInstance, a_nCmdShow);
	InitD3D();

	// init components
	m_lightManager.Init(m_pd3dDevice, m_pd3dImmDeviceContext);

	// bind samplers
	TFIEffect::InitializeSamplers(m_pd3dDevice, m_pd3dImmDeviceContext);

	// build effects from shaders
	m_pBlinnPhongFX  = new TFFXBlinnPhong(m_pd3dDevice, m_pd3dImmDeviceContext);
	m_pTerrainFX     = new TFFXTerrain(m_pd3dDevice, m_pd3dImmDeviceContext);
	m_pRenderDepthFX = new TFFXDepthRender(m_pd3dDevice, m_pd3dImmDeviceContext);
	m_pWaterStillFX  = new TFFXWaterStill(m_pd3dDevice, m_pd3dImmDeviceContext);
	m_pSkyboxFX      = new TFFXSkybox(m_pd3dDevice, m_pd3dImmDeviceContext);
	m_pFoliageFX     = new TFFXFoliage(m_pd3dDevice, m_pd3dImmDeviceContext);

	// create render to texture maps
	m_pShadowMapFront = new TFRendering::TFShadowMap(m_pd3dDevice, m_pd3dImmDeviceContext, 2048, 2048);
	m_pReflectionMap  = new TFRendering::TFReflectionMap(m_pd3dDevice, m_pd3dImmDeviceContext, 512, 512);

	// init renderables

	tfMatrix _matWorld = XMMatrixIdentity();

	// HOUSE
	m_pHouseModel = new TFModelEx(m_pd3dDevice, m_pd3dImmDeviceContext, "..\\Models\\house_obj.obj");
	_matWorld  = XMMatrixScaling(0.015f, 0.015f, 0.015f);
	_matWorld *= XMMatrixRotationAxis(m_fmCamera.GetUpVector(), XM_PIDIV2);
	_matWorld *= XMMatrixTranslation(79.0f, 54.5, -70.0f);
	m_pHouseModel->SetWorldMatrix(_matWorld);

	// SKY
	m_pSkybox = new TFModelEx(m_pd3dDevice, m_pd3dImmDeviceContext, "..\\Models\\ellipsoid.obj");
	m_pSkybox->SetWorldMatrix(XMMatrixScaling(1000.0f, 1000.0f, 1000.0f));

	// TERRAIN
	m_pTerrain = new TFTerrainEx(m_pd3dDevice, 
		m_pd3dImmDeviceContext, 
		"..\\Textures\\Terrain\\heightmap_1.bmp", 
		257, 
		257);
	_matWorld = XMMatrixRotationAxis(m_fmCamera.GetUpVector(), -XM_PI);
	m_pTerrain->SetWorldMatrix(_matWorld);

	// WATER BODIES
	m_pWater1 = new TFWaterStillEx(m_pd3dDevice,
		m_pd3dImmDeviceContext,
		50,
		50);
	_matWorld = XMMatrixTranslation(-58.0f, 39.0f, 8.0f);
	m_pWater1->SetWorldMatrix(_matWorld);

	m_pWater2 = new TFWaterStillEx(m_pd3dDevice,
		m_pd3dImmDeviceContext,
		50,
		50);
	_matWorld = XMMatrixTranslation(-8.0f, 35.0f, -74.0f);
	m_pWater2->SetWorldMatrix(_matWorld);

	// TREES
	m_pTree1 = new TFModelEx(m_pd3dDevice, m_pd3dImmDeviceContext, "..\\Models\\palm4.obj");
	_matWorld  = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	_matWorld *= XMMatrixRotationAxis(m_fmCamera.GetUpVector(), -XM_PIDIV2 + XM_PIDIV4);
	_matWorld *= XMMatrixTranslation(-51.0f, 42.0f, -14.0f);
	m_pTree1->SetWorldMatrix(_matWorld);

	m_pTree2 = new TFModelEx(m_pd3dDevice, m_pd3dImmDeviceContext, "..\\Models\\palm3.obj");
	_matWorld  = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	_matWorld *= XMMatrixRotationAxis(m_fmCamera.GetUpVector(), -XM_PIDIV2);
	_matWorld *= XMMatrixTranslation(-55, 43, 35);
	m_pTree2->SetWorldMatrix(_matWorld);

	m_pTree3 = new TFModelEx(m_pd3dDevice, m_pd3dImmDeviceContext, "..\\Models\\obj__plamt2.obj");
	_matWorld  = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	_matWorld *= XMMatrixRotationAxis(m_fmCamera.GetUpVector(), -XM_PIDIV2);
	_matWorld *= XMMatrixTranslation(18.0f, 37.0f, -76.6f);
	m_pTree3->SetWorldMatrix(_matWorld);

	// GNOME
	m_pGnome = new TFModelEx(m_pd3dDevice, m_pd3dImmDeviceContext, "..\\Models\\gnome5.obj");
	_matWorld  = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	_matWorld *= XMMatrixRotationAxis(m_fmCamera.GetUpVector(), -XM_PI);
	_matWorld *= XMMatrixTranslation(15.8f, 45.0f, 4.3f);
	m_pGnome->SetWorldMatrix(_matWorld);

	// DOOR 
	m_pDoor1 = new TFModelEx(m_pd3dDevice, m_pd3dImmDeviceContext, "..\\Models\\door.obj");
	_matWorld  = XMMatrixScaling(0.15f, 0.15f, 0.15f);
	_matWorld *= XMMatrixRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XM_PI / 5.0f);
	_matWorld *= XMMatrixTranslation(48.0f, 43.0f, 90.0f);
	m_pDoor1->SetWorldMatrix(_matWorld);
	
	// add renderables to effects
	m_pBlinnPhongFX->AddRenderable(m_pHouseModel);
	m_pBlinnPhongFX->AddRenderable(m_pGnome);
	m_pBlinnPhongFX->AddRenderable(m_pDoor1);
	m_pFoliageFX->AddRenderable(m_pTree1);
	m_pFoliageFX->AddRenderable(m_pTree2);
	m_pFoliageFX->AddRenderable(m_pTree3);
	m_pTerrainFX->AddRenderable(m_pTerrain);
	m_pRenderDepthFX->AddRenderable(m_pHouseModel);
	m_pRenderDepthFX->AddRenderable(m_pTree1);
	m_pRenderDepthFX->AddRenderable(m_pTree2);
	m_pRenderDepthFX->AddRenderable(m_pTree3);
	m_pWaterStillFX->AddRenderable(m_pWater1);
	m_pWaterStillFX->AddRenderable(m_pWater2);
	m_pSkyboxFX->AddRenderable(m_pSkybox);

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

	// Update water
	((TFFXWaterStill*)(m_pWaterStillFX))->UpdateWater(a_fDelta);

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
	m_pReflectionMap->SetRenderTarget();

	TFRenderFrontFaceCull(m_pd3dDevice, m_pd3dImmDeviceContext);

	// compute view-proj matrix
	float _fPlaneVerticalOffset = 39.0f;
	tfMatrix _matFlip = XMMatrixScaling(1.0f, -1.0f, 1.0f);
	tfMatrix _matOffset = XMMatrixTranslation(0, 2 * _fPlaneVerticalOffset, 0); // move the object up by 2 * reflection plane offset...
	tfMatrix _matViewProj = m_matView * m_matProj;

	/*** TERRAIN ***/
	tfFloat4 _f4ClipData;
	_f4ClipData.x = 39.5; // height of this reflective surface in world space
	_f4ClipData.y = 1.0f;  // whether or not to use a clip plane in the VS

	// draw terrain objects
	((TFFXTerrain*)m_pTerrainFX)->UpdatePerFrameData(_f4ClipData);
	m_pTerrainFX->BatchDraw(_matViewProj, m_lightManager.GetVPT());

	// transform view-proj matrix so that it flips all geometry after terrain and accounts for offset
	_matViewProj = _matFlip * _matOffset * m_matView * m_matProj;

	/*** BLINN-PHONG OBJECTS ***/
	m_pBlinnPhongFX->BatchDraw(_matViewProj, m_lightManager.GetVPT());

	/*** FOLIAGE ***/
	m_pFoliageFX->BatchDraw(_matViewProj, m_lightManager.GetVPT());

	/*** SKY ***/

	// update render state and depth/stencil state for ellipsoid
	TFRenderNoCull(m_pd3dDevice, m_pd3dImmDeviceContext);
	TFSetDepthLessEqual(m_pd3dDevice, m_pd3dImmDeviceContext);

	m_pSkyboxFX->BatchDraw(_matViewProj, m_lightManager.GetVPT());

	// restore default states
	m_pd3dImmDeviceContext->RSSetState(0);
	m_pd3dImmDeviceContext->OMSetDepthStencilState(0, 0);

	/*** Reset rasterizer and render target ***/
	m_pd3dImmDeviceContext->RSSetState(0);
	ResetRenderTarget();
}

void TFDemo2Driver::RenderScene()
{
	TFCore::TFWinBase::RenderScene();

	RenderToShadowMap();
	RenderToReflectionMap();

	//TFRenderWireframe(m_pd3dDevice, m_pd3dImmDeviceContext);

	// compute view-proj matrix
	tfMatrix _matViewProj = m_matView * m_matProj;

	/*** BLINN-PHONG MODELS ***/
	m_pBlinnPhongFX->SetShadowMap(m_pShadowMapFront->GetDepthMapSRV());
	m_pBlinnPhongFX->BatchDraw(_matViewProj, m_lightManager.GetVPT());

	/*** FOLIAGE ***/
	m_pFoliageFX->SetShadowMap(m_pShadowMapFront->GetDepthMapSRV());
	m_pFoliageFX->BatchDraw(_matViewProj, m_lightManager.GetVPT());

	/*** TERRAIN ***/
	tfFloat4 _f4ClipData;
	memset(&_f4ClipData, 0, sizeof(tfFloat4));

	((TFFXTerrain*)m_pTerrainFX)->UpdatePerFrameData(_f4ClipData);
	m_pTerrainFX->SetShadowMap(m_pShadowMapFront->GetDepthMapSRV());
	m_pTerrainFX->BatchDraw(_matViewProj, m_lightManager.GetVPT());
	m_pTerrainFX->UnbindShadowMap();

	/*** WATER ***/
	m_pWaterStillFX->SetReflectionMap(m_pReflectionMap->GetReflectionMapSRV());
	m_pWaterStillFX->SetBlendState();
	m_pWaterStillFX->BatchDraw(_matViewProj, m_lightManager.GetVPT());
	m_pWaterStillFX->UnbindReflectionMap();

	// reset blend state
	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pd3dImmDeviceContext->OMSetBlendState(NULL, blendFactors, 0xffffffff); 

	/*** SKY ***/

	// update render state and depth/stencil state for ellipsoid
	TFRenderNoCull(m_pd3dDevice, m_pd3dImmDeviceContext);
	TFSetDepthLessEqual(m_pd3dDevice, m_pd3dImmDeviceContext);

	m_pSkyboxFX->BatchDraw(_matViewProj, m_lightManager.GetVPT());

	// restore default states
	m_pd3dImmDeviceContext->RSSetState(0);
	m_pd3dImmDeviceContext->OMSetDepthStencilState(0, 0);

	/*** DISPLAY ***/

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
