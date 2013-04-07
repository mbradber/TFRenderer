#include "TFDriver.h"
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
	// Delete renderable objects
	delete m_pShadowMap;
}

void TFApplication::Init(HINSTANCE hInstance, int nCmdShow)
{
	InitWindowsApp(hInstance, nCmdShow);
	InitD3D();

	m_lightManager.Init(m_pd3dDevice, m_pd3dImmDeviceContext);

	m_shaderManager.Init(m_pd3dDevice);

	m_shaderManager.AddVertexShader(L"NormalMapping", L"..\\Debug\\NormalMapVS.cso", TFPosNormTexTanLayout, 4);
	m_shaderManager.AddPixelShader(L"NormalMapping", L"..\\Debug\\NormalMapPS.cso");

	m_shaderManager.AddVertexShader(L"RenderDepth", L"..\\Debug\\RenderDepthVS.cso", TFPosNormTexTanLayout, 4);
	m_shaderManager.AddPixelShader(L"RenderDepth", L"..\\Debug\\RenderDepthPS.cso");

	// Bind the default sampler state to the PS
	ID3D11SamplerState* _defaultSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_ANISOTROPIC);
	m_pd3dImmDeviceContext->PSSetSamplers(0, 1, &_defaultSampler);


	ID3D11VertexShader* _pNormalMapVS            = m_shaderManager.GetVertexShaderByName(L"NormalMapping");
	ID3D11PixelShader*  _pNormalMapPS            = m_shaderManager.GetPixelShaderByName(L"NormalMapping");
	ID3D11InputLayout*  _pNormalMapInputLayout   = m_shaderManager.GetInputLayoutByName(L"NormalMapping");

	ID3D11VertexShader* _pRenderDepthVS          = m_shaderManager.GetVertexShaderByName(L"RenderDepth");
	ID3D11PixelShader*  _pRenderDepthPS          = m_shaderManager.GetPixelShaderByName(L"RenderDepth");
	ID3D11InputLayout*  _pRenderDepthInputLayout = m_shaderManager.GetInputLayoutByName(L"RenderDepth");


	// Init shadow map
	m_pShadowMap = new TFRendering::TFShadowMap(m_pd3dDevice, m_pd3dImmDeviceContext, 2048, 2048);


	//m_model.Init(m_pd3dDevice, m_pd3dImmDeviceContext, 1.0f, m_shaderManager.GetActiveVertexShader(),
	//	m_shaderManager.GetActivePixelShader(), m_shaderManager.GetActiveInputLayout(), "..\\Models\\tree1_3ds\\Tree1.3ds");



	// cube map stuff (move somewhere else later)

	m_box1.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		1.0f, 
		_pNormalMapVS,
		_pNormalMapPS,
		_pNormalMapInputLayout,
		"..\\Models\\Crate_Fragile.obj");

	m_box1.AddShadowShaders(_pRenderDepthVS,
		_pRenderDepthPS,
		_pRenderDepthInputLayout);

	m_box2.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		1.0f, 
		_pNormalMapVS,
		_pNormalMapPS,
		_pNormalMapInputLayout,
		"..\\Models\\Crate_Fragile.obj");


	// Set up initial matrices for WVP
	m_matWorld = XMMatrixIdentity();
	m_matProj  = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_nClientWidth / static_cast<float>(m_nClientHeight), 0.01f, 100.0f);
	m_matView  = XMMatrixLookAtLH(XMVectorSet(-5.0f, 0.0f, -5.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	// light source view matrix
	m_matLightView = XMMatrixIdentity();
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

void TFApplication::RenderToShadowMap()
{
	//// Set the render state for depth bias rendering into shadow map
	TFDepthBiasRender(m_pd3dDevice, m_pd3dImmDeviceContext);

	// Set viewport to shadow map viewport, set render target to null (Disables color writes)
	// and set the depth draws to shadow map depth stencil view
	m_pShadowMap->SetRenderState();

	// Set world matrix for first box
	m_matWorld = XMMatrixScaling(0.2f, 0.2f, 0.2f);

	// Update the geometry with their respective transforms
	XMMATRIX _matWVP = m_matWorld * m_lightManager.GetView() * m_lightManager.GetProjection();

	m_box1.UpdateShadowResources(_matWVP);
	m_box1.ActivateShadowShaders();
	m_box1.Draw();

	// restore default render states
	m_pd3dImmDeviceContext->RSSetState(0);
	m_pd3dImmDeviceContext->OMSetDepthStencilState(0, 0);

	// reset render target
	ResetRenderTarget();
}

void TFApplication::RenderScene()
{
	TFCore::TFWinBase::RenderScene();

	// build shadow map
	RenderToShadowMap();

	//TFRenderWireframe(m_pd3dDevice, m_pd3dImmDeviceContext);

	// Set world matrix for first box
	m_matWorld = XMMatrixScaling(0.2f, 0.2f, 0.2f);

	// Update the geometry with their respective transforms
	XMMATRIX _matWVP = m_matWorld * m_matView * m_matProj;

	m_box1.UpdateResources(_matWVP, m_matWorld, XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_box1.ActivateShaders();
	m_box1.Draw();

	// update geometry of box 2 (ground)
	m_matWorld = XMMatrixScaling(2.0f, 0.2f, 2.0f);
	m_matWorld *= XMMatrixTranslation(0.0f, -7.9f, 0.0f);
	_matWVP = m_matWorld * m_matView * m_matProj;

	m_box2.UpdateResources(_matWVP, m_matWorld, XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_box2.ActivateShaders();
	m_box2.Draw();

	// restore default states
	m_pd3dImmDeviceContext->RSSetState(0);
	m_pd3dImmDeviceContext->OMSetDepthStencilState(0, 0);

	// Display the back buffer
	m_pSwapChain->Present( 1, 0 );
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, int nCmdShow)
{
	TFApplication _tfApp;
	_tfApp.Init(hInstance, nCmdShow);
	_tfApp.Run();

	return 0;
}