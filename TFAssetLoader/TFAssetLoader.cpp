#include "TFAssetLoader.h"
#include "TFMath.h"
#include "TFInput.h"
#include "TFVertices.h"
#include "TFUtils.h"
#include "TFModel.h"

using namespace TFCore;

TFApplication::TFApplication()
{

}

TFApplication::~TFApplication()
{

}

void TFApplication::Init(HINSTANCE hInstance, int nCmdShow)
{
	InitWindowsApp(hInstance, nCmdShow);
	InitD3D();

	

	m_lightManager.Init(m_pd3dDevice, m_pd3dImmDeviceContext);

	m_shaderManager.Init(m_pd3dDevice);
	m_shaderManager.SetActiveVertexShader(L"..\\Debug\\SimpleDirLightVS.cso", TFPositionNormalTextureLayout, 3);
	m_shaderManager.SetActivePixelShader(L"..\\Debug\\SimpleDirLightPS.cso");

	// Bind the default sampler state to the PS
	ID3D11SamplerState* _defaultSampler = m_shaderManager.GetSamplerState();
	m_pd3dImmDeviceContext->PSSetSamplers(0, 1, &_defaultSampler );

	m_spider.Init(m_pd3dDevice, 
		m_pd3dImmDeviceContext, 
		1.0f, 
		m_shaderManager.GetActiveVertexShader(),
		m_shaderManager.GetActivePixelShader(), 
		m_shaderManager.GetActiveInputLayout(), 
		"..\\Models\\spider.irrmesh");

	m_terrain.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		1.0f, 
		m_shaderManager.GetActiveVertexShader(),
		m_shaderManager.GetActivePixelShader(), 
		m_shaderManager.GetActiveInputLayout(), 
		"..\\Models\\Simplecube.obj");

	m_tree1.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		1.0f, 
		m_shaderManager.GetActiveVertexShader(),
		m_shaderManager.GetActivePixelShader(), 
		m_shaderManager.GetActiveInputLayout(), 
		"..\\Models\\tree1\\Tree1.3ds");

	m_zard1.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		1.0f, 
		m_shaderManager.GetActiveVertexShader(),
		m_shaderManager.GetActivePixelShader(), 
		m_shaderManager.GetActiveInputLayout(), 
		"..\\Models\\zard.obj");

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

	// Update the lights
	//m_lightManager.Update(a_fDelta, m_fmCamera.GetPosition());
}

void TFApplication::RenderScene()
{
	TFCore::TFWinBase::RenderScene();

	//TFRenderWireframe(m_pd3dDevice, m_pd3dImmDeviceContext);

	// Set world matrix for first model
	m_matWorld = XMMatrixScaling(0.03f, 0.03f, 0.03f);
	m_matWorld = m_matWorld * XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XM_PIDIV2);
	m_matWorld = m_matWorld * XMMatrixTranslation(4.0f, 1.4f, 0.0f);

	// Update the geometry with their respective transforms
	XMMATRIX _matWVP = m_matWorld * m_matView * m_matProj;

	m_spider.UpdateResources(_matWVP, m_matWorld, XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_spider.ActivateShaders();
	m_spider.Draw();

	// Set world matrix for second model
	m_matWorld = XMMatrixTranslation(0.0f, -2.0f, 0.0f);
	m_matWorld = m_matWorld * XMMatrixScaling(10.0f, 1.0f, 10.0f);
	_matWVP = m_matWorld * m_matView * m_matProj;
	m_terrain.UpdateResources(_matWVP, m_matWorld, XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_terrain.ActivateShaders();
	m_terrain.Draw();

	// Set the world matrix for the third model
	m_matWorld = XMMatrixRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XM_PIDIV2);
	_matWVP = m_matWorld * m_matView * m_matProj;
	m_tree1.UpdateResources(_matWVP, m_matWorld, XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_tree1.ActivateShaders();
	m_tree1.Draw();

	// Set the world matrix for the fourth model
	m_matWorld = XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), -1.047);
	m_matWorld = m_matWorld * XMMatrixTranslation(-7.0f, 2.5f, 2.0f);
	_matWVP = m_matWorld * m_matView * m_matProj;
	m_zard1.UpdateResources(_matWVP, m_matWorld, XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_zard1.ActivateShaders();
	m_zard1.Draw();

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