#include "TFDemo2Driver.h"
#include "TFMath.h"
#include "TFInput.h"
#include "TFVertices.h"
#include "TFUtils.h"
#include "TFModel.h"
#include <sstream>
#include <string>
#include "TFModelEx.h"

using namespace TFCore;
using namespace TFRendering;
using namespace std;
using namespace DirectX;

TFDemo2Driver::TFDemo2Driver(void)
{
}


TFDemo2Driver::~TFDemo2Driver(void)
{
	delete m_pEffect1;
	delete m_pModelEx1;
}

void TFDemo2Driver::Init(HINSTANCE hInstance, int a_nCmdShow)
{
	InitWindowsApp(hInstance, a_nCmdShow);
	InitD3D();

	// init components
	m_lightManager.Init(m_pd3dDevice, m_pd3dImmDeviceContext);
	m_shaderManager.Init(m_pd3dDevice);
	
#ifdef _DEBUG
	wstring _wsShaderPrefix = L"..\\Debug\\";

#else
	wstring _wsShaderPrefix = L"..\\Release\\";
#endif

	// build effects from shaders
	m_pEffect1 = new TFBlinnPhong();
	m_pEffect1->Initialize(m_pd3dDevice,
		m_pd3dImmDeviceContext,
		L"ShadowsVS.cso",
		_wsShaderPrefix);

	m_pEffect1->AddPixelShader(L"ShadowsPS.cso");

	// bind samplers
	ID3D11SamplerState* _defaultSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_ANISOTROPIC);
	m_pd3dImmDeviceContext->PSSetSamplers(0, 1, &_defaultSampler);
	_defaultSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_TRILINEAR);
	m_pd3dImmDeviceContext->PSSetSamplers(1, 1, &_defaultSampler);
	_defaultSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_SHADOW);
	m_pd3dImmDeviceContext->PSSetSamplers(2, 1, &_defaultSampler);
	_defaultSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_POINT);
	m_pd3dImmDeviceContext->PSSetSamplers(3, 1, &_defaultSampler);

	tfMatrix _matWorld = XMMatrixIdentity();
	
	// init renderables
	m_pModelEx1 = new TFModelEx(m_pd3dDevice, m_pd3dImmDeviceContext, "..\\Models\\house_obj.obj");
	_matWorld  = XMMatrixScaling(0.015f, 0.015f, 0.015f);
	m_pModelEx1->SetWorldMatrix(_matWorld);
	
	// add renderables to effects
	m_pEffect1->AddRenderable(m_pModelEx1);





	// Set up initial matrices for WVP
	m_matWorld = XMMatrixIdentity();
	m_matProj  = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_nClientWidth / static_cast<float>(m_nClientHeight), 1.0f, 1000.0f);
	m_matView  = XMMatrixLookAtLH(XMVectorSet(-5.0f, 0.0f, -5.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	// light source view matrix
	m_matLightView = XMMatrixIdentity();

	// Update the lights
	m_lightManager.Update(1.0f, m_fmCamera.GetPosition(), true);
}

void TFDemo2Driver::OnResize()
{
	TFWinBase::OnResize();
	// Rebuild projection matrix on window resize
	m_matProj = TFMatrixPerspectiveLH(XM_PIDIV4, m_nClientWidth / static_cast<float>(m_nClientHeight), 1.0f, 1000.0f);
}

void TFDemo2Driver::Run()
{
	TFCore::TFWinBase::Run();
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

	///*** HOUSE ***/
	//m_matWorld = m_house1.GetWorldMatrix();
	//_matWVP = m_matWorld * _matViewProj;

	//m_house1.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	//m_house1.ActivateShaders();
	//m_house1.Draw();

	m_pEffect1->BatchDraw(_matViewProj, m_lightManager.GetVPT());


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
