//#include "TFDriver.h"
//#include "TFMath.h"
//#include "TFInput.h"
//#include "TFVertices.h"
//#include "TFUtils.h"
//#include "TFModel.h"
//
//
//using namespace TFCore;
//
//TFApplication::TFApplication()
//	:m_pCube1(NULL),
//	 m_pCube2(NULL),
//	 m_pGround1(NULL)
//{
//	m_pCube1   = new TFCube();
//	m_pCube2   = new TFCube();
//	m_pGround1 = new TFCube();
//}
//
//TFApplication::~TFApplication()
//{
//	// Delete renderable objects
//	delete m_pShadowMap;
//	delete m_pCube1;
//	delete m_pCube2;
//	delete m_pGround1;
//}
//
//void TFApplication::Init(HINSTANCE hInstance, int nCmdShow)
//{
//	InitWindowsApp(hInstance, nCmdShow);
//	InitD3D();
//
//	m_lightManager.Init(m_pd3dDevice, m_pd3dImmDeviceContext);
//
//	m_shaderManager.Init(m_pd3dDevice);
//
//	m_shaderManager.AddVertexShader(L"SimpleDirLight", L"..\\Debug\\SimpleDirLightVS.cso", TFPositionNormalTextureLayout, 3);
//	m_shaderManager.AddPixelShader(L"SimpleDirLight", L"..\\Debug\\SimpleDirLightPS.cso");
//
//	m_shaderManager.AddVertexShader(L"Shadows", L"..\\Debug\\ShadowsVS.cso", TFPosNormTexTanLayout, 4);
//	m_shaderManager.AddPixelShader(L"Shadwos", L"..\\Debug\\ShadowsPS.cso");
//
//	m_shaderManager.AddVertexShader(L"Sky", L"..\\Debug\\SkyVS.cso", TFSimplePositionLayout, 1);
//	m_shaderManager.AddPixelShader(L"Sky", L"..\\Debug\\SkyPS.cso");
//
//	// Bind the default sampler state to the PS
//	ID3D11SamplerState* _defaultSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_ANISOTROPIC);
//	m_pd3dImmDeviceContext->PSSetSamplers(0, 1, &_defaultSampler);
//
//
//	ID3D11VertexShader* _pSimpleDirVS = m_shaderManager.GetVertexShaderByName(L"SimpleDirLight");
//	ID3D11PixelShader*  _pSimpleDirPS = m_shaderManager.GetPixelShaderByName(L"SimpleDirLight");
//	ID3D11InputLayout*  _pSimpleDirInputLayout = m_shaderManager.GetInputLayoutByName(L"SimpleDirLight");
//
//	ID3D11VertexShader* _pSkyVS = m_shaderManager.GetVertexShaderByName(L"Sky");
//	ID3D11PixelShader*  _pSkyPS = m_shaderManager.GetPixelShaderByName(L"Sky");
//	ID3D11InputLayout*  _pSkyInputLayout = m_shaderManager.GetInputLayoutByName(L"Sky");
//
//	// Init shadow map
//	m_pShadowMap = new TFRendering::TFShadowMap(m_pd3dDevice, m_pd3dImmDeviceContext, 2048, 2048);
//
//
//	//m_model.Init(m_pd3dDevice, m_pd3dImmDeviceContext, 1.0f, m_shaderManager.GetActiveVertexShader(),
//	//	m_shaderManager.GetActivePixelShader(), m_shaderManager.GetActiveInputLayout(), "..\\Models\\tree1_3ds\\Tree1.3ds");
//
//	m_pGround1->Init(
//		m_pd3dDevice, 
//		m_pd3dImmDeviceContext, 
//		_pSimpleDirVS,
//		_pSimpleDirPS,
//		_pSimpleDirInputLayout,
//		"..\\Textures\\grass.dds");
//
//	m_pCube1->Init(
//		m_pd3dDevice, 
//		m_pd3dImmDeviceContext, 
//		_pSimpleDirVS,
//		_pSimpleDirPS,
//		_pSimpleDirInputLayout,
//		"..\\Textures\\WoodCrate01.dds");
//
//	m_pCube2->Init(
//		m_pd3dDevice, 
//		m_pd3dImmDeviceContext, 
//		_pSimpleDirVS,
//		_pSimpleDirPS,
//		_pSimpleDirInputLayout,
//		"..\\Textures\\WoodCrate02.dds");
//
//	// cube map stuff (move somewhere else later)
//
//	m_ellipsoid.Init(m_pd3dDevice, 
//		m_pd3dImmDeviceContext, 
//		_pSkyVS,
//		_pSkyPS,
//		_pSkyInputLayout,
//		"..\\Models\\SimpleSphere.obj");
//
//	HR(D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, L"..\\Textures\\snowcube1024.dds", 0, 0, &m_cubeMapSRV, 0));
//
//	// describe the cb for the WVP matrix
//	D3D11_BUFFER_DESC bd;
//	ZeroMemory(&bd, sizeof(bd));
//	bd.Usage          = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth      = sizeof(XMMATRIX);
//	bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
//	bd.CPUAccessFlags = 0;
//	bd.MiscFlags      = 0;
//
//	// Create the constant buffer with the device
//	HR(m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pEllipsoidCB));
//
//
//	// Set up initial matrices for WVP
//	m_matWorld = XMMatrixIdentity();
//	m_matProj  = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_nClientWidth / static_cast<float>(m_nClientHeight), 0.01f, 100.0f);
//	m_matView  = XMMatrixLookAtLH(XMVectorSet(-5.0f, 0.0f, -5.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
//
//	// light source view matrix
//	m_matLightView = XMMatrixIdentity();
//}
//
//void TFApplication::OnResize()
//{
//	TFWinBase::OnResize();
//	// Rebuild projection matrix on window resize
//	m_matProj = TFMatrixPerspectiveLH(XM_PIDIV4, m_nClientWidth / static_cast<float>(m_nClientHeight), 0.01f, 100.0f);
//}
//
//void TFApplication::Run()
//{
//	TFCore::TFWinBase::Run();
//}
//
//void TFApplication::UpdateScene(float a_fDelta)
//{
//	TFCore::TFWinBase::UpdateScene(a_fDelta);
//
//	// Update camera (process user input)
//	m_fmCamera.Update(a_fDelta);
//
//	// Grab view matrix from the camera
//	m_matView = m_fmCamera.GetView();
//
//	// Update the lights
//	m_lightManager.Update(a_fDelta, m_fmCamera.GetPosition());
//}
//
//void TFApplication::RenderToShadowMap()
//{
//	//// Set the render state for depth bias rendering into shadow map
//	TFDepthBiasRender(m_pd3dDevice, m_pd3dImmDeviceContext);
//
//	// Set world matrix for first box
//	m_matWorld = XMMatrixTranslation(2.0f, 0.0f, 0.0f);
//
//	// Update the geometry with their respective transforms
//	XMMATRIX _matWVP = m_matWorld * m_lightManager.GetView() * m_lightManager.GetProjection();
//
//	//m_pCube1
//
//	//XMMATRIX _lightVPT = m_lightManager.GetVPT();
//	//XMMATRIX _matWVPT = m_matWorld * _lightVPT;
//
//	//// update consant 
//	//ID3D11Buffer* _pPerObjectCB;
//	//// describe the cb for the WVP matrix
//	//D3D11_BUFFER_DESC bd;
//	//ZeroMemory(&bd, sizeof(bd));
//	//bd.Usage          = D3D11_USAGE_DEFAULT;
//	//bd.ByteWidth      = sizeof(TFBufferPerObject);
//	//bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
//	//bd.CPUAccessFlags = 0;
//	//bd.MiscFlags      = 0;
//
//	//// Create the constant buffer with the device
//	//HR(m_pd3dDevice->CreateBuffer(&bd, NULL, &_pPerObjectCB));
//
//
//
//
//
//	//// restore default states
//	//m_pd3dImmDeviceContext->RSSetState(0);
//	//m_pd3dImmDeviceContext->OMSetDepthStencilState(0, 0);
//}
//
//void TFApplication::RenderScene()
//{
//	TFCore::TFWinBase::RenderScene();
//
//	RenderToShadowMap();
//
//	//TFRenderWireframe(m_pd3dDevice, m_pd3dImmDeviceContext);
//
//	// Set world matrix for first box
//	m_matWorld = XMMatrixTranslation(2.0f, 0.0f, 0.0f);
//
//	// Update the geometry with their respective transforms
//	XMMATRIX _matWVP = m_matWorld * m_matView * m_matProj;
//
//	// Update and activate the shaders, then draw the geometry
//	m_pCube1->UpdateResources(_matWVP, m_matWorld, XMMatrixIdentity(), m_fmCamera.GetPosition());
//	m_pCube1->ActivateShaders();
//	m_pCube1->Draw();
//
//	// Set world matrix for second box
//	m_matWorld = XMMatrixTranslation(-2.0f, 0.0f, 0.0f);
//
//	// Update the geometry with their respective transforms
//	_matWVP = m_matWorld * m_matView * m_matProj;
//
//	// Update and activate the shaders, then draw the geometry
//	m_pCube2->UpdateResources(_matWVP, m_matWorld, XMMatrixIdentity(), m_fmCamera.GetPosition());
//	m_pCube2->ActivateShaders();
//	m_pCube2->Draw();
//
//	// set world matrix for model
//	//m_matWorld = XMMatrixIdentity();
//	//_matWVP = m_matWorld * m_matView * m_matProj;
//	//m_model.UpdateResources(_matWVP, m_matWorld, XMMatrixIdentity(), m_fmCamera.GetPosition());
//	//m_model.ActivateShaders();
//	//m_model.Draw();
//
//	// Set world matrix for ground
//
//	m_matWorld = XMMatrixRotationX(XM_PIDIV2);
//	m_matWorld *= XMMatrixScaling(10.0f, 1.0f, 14.0f);
//	m_matWorld *= XMMatrixTranslation(0.0f, -2.0f, 0.0f);
//
//	// Construct a transform for the ground's texture coordinates so the single 
//	// texture doesn't stretch across the whole thing.
//	XMMATRIX _matTexTransform = XMMatrixScaling(10.0f, 10.0f, 0.0f);
//
//	// Update the geometry with their respective transforms
//	_matWVP = m_matWorld * m_matView * m_matProj;
//
//	// Update and activate the shaders, then draw the geometry
//	m_pGround1->UpdateResources(_matWVP, m_matWorld, _matTexTransform, m_fmCamera.GetPosition());
//	m_pGround1->ActivateShaders();
//	m_pGround1->Draw();
//
//	// Set world matrix for first model
//	m_matWorld = XMMatrixIdentity();
//	m_matWorld = m_matWorld * XMMatrixScaling(100.0f, 100.0f, 100.0f);
//
//	// update render state and depth/stencil state for ellipsoid
//	TFRenderNoCull(m_pd3dDevice, m_pd3dImmDeviceContext);
//	TFSetDepthLessEqual(m_pd3dDevice, m_pd3dImmDeviceContext);
//
//	// set sampler state for cube map
//	ID3D11SamplerState* _trilinearSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_TRILINEAR);
//	m_pd3dImmDeviceContext->PSSetSamplers(0, 1, &_trilinearSampler );
//
//	// Update the geometry with their respective transforms
//	_matWVP = m_matWorld * m_matView * m_matProj;
//
//	//m_ellipsoid.UpdateResources(_matWVP, m_matWorld, XMMatrixIdentity(), m_fmCamera.GetPosition());
//	//m_ellipsoid.ActivateShaders();
//
//	XMMATRIX _matEllipseWVP = XMMatrixTranspose(_matWVP);
//	m_pd3dImmDeviceContext->UpdateSubresource(m_pEllipsoidCB, 0, NULL, &_matEllipseWVP, 0, 0);
//
//	m_pd3dImmDeviceContext->VSSetShader(m_ellipsoid.GetVertexShader(), NULL, 0);
//	m_pd3dImmDeviceContext->PSSetShader(m_ellipsoid.GetPixelShader(), NULL, 0); 
//
//	m_pd3dImmDeviceContext->VSSetConstantBuffers(0, 1, &m_pEllipsoidCB);
//	m_pd3dImmDeviceContext->PSSetShaderResources(0, 1, &m_cubeMapSRV);
//	m_pd3dImmDeviceContext->IASetInputLayout(m_ellipsoid.GetInputLayout());
//
//	m_ellipsoid.Draw();
//
//	// restore default states
//	m_pd3dImmDeviceContext->RSSetState(0);
//	m_pd3dImmDeviceContext->OMSetDepthStencilState(0, 0);
//
//	ID3D11SamplerState* _anisoSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_ANISOTROPIC);
//	m_pd3dImmDeviceContext->PSSetSamplers(0, 1, &_anisoSampler );
//
//	// Display the back buffer
//	m_pSwapChain->Present( 0, 0 );
//}
//
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, int nCmdShow)
{
	//TFApplication _tfApp;
	//_tfApp.Init(hInstance, nCmdShow);
	//_tfApp.Run();

	return 0;
}