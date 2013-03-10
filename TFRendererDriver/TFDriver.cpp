#include "TFDriver.h"
#include "TFMath.h"
#include "TFInput.h"
#include "TFVertices.h"
#include "TFUtils.h"

using namespace TFCore;

TFApplication::TFApplication()
	:m_pCube1(NULL),
	 m_pCube2(NULL),
	 m_pGround1(NULL)
{
	m_pCube1   = new TFCube();
	m_pCube2   = new TFCube();
	m_pGround1 = new TFCube();
}

TFApplication::~TFApplication()
{
	// Delete renderable objects
	delete m_pCube1;
	delete m_pCube2;
	delete m_pGround1;
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

	m_pGround1->Init(
		m_pd3dDevice, 
		m_pd3dImmDeviceContext, 
		1.0f, 
		m_shaderManager.GetActiveVertexShader(), 
		m_shaderManager.GetActivePixelShader(), 
		m_shaderManager.GetActiveInputLayout(), 
		L"..\\Textures\\grass.dds");

	m_pCube1->Init(
		m_pd3dDevice, 
		m_pd3dImmDeviceContext, 
		1.0f, 
		m_shaderManager.GetActiveVertexShader(), 
		m_shaderManager.GetActivePixelShader(), 
		m_shaderManager.GetActiveInputLayout(), 
		L"..\\Textures\\WoodCrate01.dds");

	m_pCube2->Init(
		m_pd3dDevice, 
		m_pd3dImmDeviceContext, 
		1.0f, 
		m_shaderManager.GetActiveVertexShader(), 
		m_shaderManager.GetActivePixelShader(), 
		m_shaderManager.GetActiveInputLayout(), 
		L"..\\Textures\\WoodCrate02.dds");


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
	m_lightManager.Update(a_fDelta, m_fmCamera.GetPosition());
}

void TFApplication::RenderScene()
{
	TFCore::TFWinBase::RenderScene();

	//TFRenderWireframe(m_pd3dDevice, m_pd3dImmDeviceContext);

	// Set world matrix for first box
	m_matWorld = XMMatrixTranslation(2.0f, 0.0f, 0.0f);

	// Update the geometry with their respective transforms
	XMMATRIX _matWVP = m_matWorld * m_matView * m_matProj;

	// Update and activate the shaders, then draw the geometry
	m_pCube1->UpdateResources(_matWVP, m_matWorld, XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_pCube1->ActivateShaders();
	m_pCube1->Draw();

	// Set world matrix for second box
	m_matWorld = XMMatrixTranslation(-2.0f, 0.0f, 0.0f);

	// Update the geometry with their respective transforms
	_matWVP = m_matWorld * m_matView * m_matProj;

	// Update and activate the shaders, then draw the geometry
	m_pCube2->UpdateResources(_matWVP, m_matWorld, XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_pCube2->ActivateShaders();
	m_pCube2->Draw();

	// Set world matrix for ground

	m_matWorld = XMMatrixRotationX(XM_PIDIV2);
	m_matWorld *= XMMatrixScaling(10.0f, 1.0f, 14.0f);
	m_matWorld *= XMMatrixTranslation(0.0f, -2.0f, 0.0f);

	// Construct a transform for the ground's texture coordinates so the single 
	// texture doesn't stretch across the whole thing.
	XMMATRIX _matTexTransform = XMMatrixScaling(10.0f, 10.0f, 0.0f);

	// Update the geometry with their respective transforms
	_matWVP = m_matWorld * m_matView * m_matProj;

	// Update and activate the shaders, then draw the geometry
	m_pGround1->UpdateResources(_matWVP, m_matWorld, _matTexTransform, m_fmCamera.GetPosition());
	m_pGround1->ActivateShaders();
	m_pGround1->Draw();

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