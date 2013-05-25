#include "TFDemoDriver.h"
#include "TFMath.h"
#include "TFInput.h"
#include "TFVertices.h"
#include "TFUtils.h"
#include "TFModel.h"
#include <sstream>

using namespace TFCore;
using namespace std;

TFDemoDriver::TFDemoDriver(void)
{
}


TFDemoDriver::~TFDemoDriver(void)
{
	// Delete renderable objects
	delete m_pShadowMapFront;
	delete m_pReflectionMap;
}

void TFDemoDriver::Init(HINSTANCE hInstance, int a_nCmdShow)
{
	InitWindowsApp(hInstance, a_nCmdShow);
	InitD3D();

	// init components
	m_lightManager.Init(m_pd3dDevice, m_pd3dImmDeviceContext);
	m_shaderManager.Init(m_pd3dDevice);

	// register shaders 
	// TODO: don't always use the debug shaders...
	m_shaderManager.AddVertexShader(L"NormalMapping", L"..\\Debug\\NormalMapVS.cso", TFPosNormTexTanLayout, 4);
	m_shaderManager.AddPixelShader(L"NormalMapping", L"..\\Debug\\NormalMapPS.cso");

	m_shaderManager.AddVertexShader(L"RenderDepth", L"..\\Debug\\RenderDepthVS.cso", TFPosNormTexTanLayout, 4);
	m_shaderManager.AddPixelShader(L"RenderDepth", L"..\\Debug\\RenderDepthPS.cso");

	m_shaderManager.AddVertexShader(L"Shadows", L"..\\Debug\\ShadowsVS.cso", TFPosNormTexTanLayout, 4);
	m_shaderManager.AddPixelShader(L"Shadows", L"..\\Debug\\ShadowsPS.cso");

	m_shaderManager.AddVertexShader(L"Terrain", L"..\\Debug\\TerrainVS.cso", TFPosNormTex4TanLayout, 4);
	m_shaderManager.AddPixelShader(L"Terrain", L"..\\Debug\\TerrainPS.cso");

	m_shaderManager.AddVertexShader(L"StillWater", L"..\\Debug\\StillWaterVS.cso", TFPosNormTex4TanLayout, 4);
	m_shaderManager.AddPixelShader(L"StillWater", L"..\\Debug\\StillWaterPS.cso");

	m_shaderManager.AddVertexShader(L"Sky", L"..\\Debug\\SkyVS.cso", TFPosNormTexTanLayout, 4);
	m_shaderManager.AddPixelShader(L"Sky", L"..\\Debug\\SkyPS.cso");

	// bind samplers
	ID3D11SamplerState* _defaultSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_ANISOTROPIC);
	m_pd3dImmDeviceContext->PSSetSamplers(0, 1, &_defaultSampler);
	_defaultSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_TRILINEAR);
	m_pd3dImmDeviceContext->PSSetSamplers(1, 1, &_defaultSampler);
	_defaultSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_SHADOW);
	m_pd3dImmDeviceContext->PSSetSamplers(2, 1, &_defaultSampler);
	_defaultSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_POINT);
	m_pd3dImmDeviceContext->PSSetSamplers(3, 1, &_defaultSampler);

	// get handles to shaders and input layouts
	ID3D11VertexShader* _pNormalMapVS            = m_shaderManager.GetVertexShaderByName(L"NormalMapping");
	ID3D11PixelShader*  _pNormalMapPS            = m_shaderManager.GetPixelShaderByName(L"NormalMapping");
	ID3D11InputLayout*  _pNormalMapInputLayout   = m_shaderManager.GetInputLayoutByName(L"NormalMapping");

	ID3D11VertexShader* _pRenderDepthVS          = m_shaderManager.GetVertexShaderByName(L"RenderDepth");
	ID3D11PixelShader*  _pRenderDepthPS          = m_shaderManager.GetPixelShaderByName(L"RenderDepth");
	ID3D11InputLayout*  _pRenderDepthInputLayout = m_shaderManager.GetInputLayoutByName(L"RenderDepth");

	ID3D11VertexShader* _pShadowsVS              = m_shaderManager.GetVertexShaderByName(L"Shadows");
	ID3D11PixelShader*  _pShadowsPS              = m_shaderManager.GetPixelShaderByName( L"Shadows");
	ID3D11InputLayout*  _pShadowsInputLayout     = m_shaderManager.GetInputLayoutByName( L"Shadows");

	ID3D11VertexShader* _pTerrainVS              = m_shaderManager.GetVertexShaderByName(L"Terrain");
	ID3D11PixelShader*  _pTerrainPS              = m_shaderManager.GetPixelShaderByName( L"Terrain");
	ID3D11InputLayout*  _pTerrainInputLayout     = m_shaderManager.GetInputLayoutByName( L"Terrain");

	ID3D11VertexShader* _pStillWaterVS           = m_shaderManager.GetVertexShaderByName(L"StillWater");
	ID3D11PixelShader*  _pStillWaterPS           = m_shaderManager.GetPixelShaderByName( L"StillWater");
	ID3D11InputLayout*  _pStillWaterInputLayout  = m_shaderManager.GetInputLayoutByName( L"StillWater");

	ID3D11VertexShader* _pSkyVS                  = m_shaderManager.GetVertexShaderByName(L"Sky");
	ID3D11PixelShader*  _pSkyPS                  = m_shaderManager.GetPixelShaderByName( L"Sky");
	ID3D11InputLayout*  _pSkyInputLayout         = m_shaderManager.GetInputLayoutByName( L"Sky");

	// create render to texture maps
	m_pShadowMapFront = new TFRendering::TFShadowMap(m_pd3dDevice, m_pd3dImmDeviceContext, 2048, 2048);
	m_pReflectionMap  = new TFRendering::TFReflectionMap(m_pd3dDevice, m_pd3dImmDeviceContext, 512, 512);

	// init models
	m_box1.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		1.0f, 
		_pShadowsVS,
		_pShadowsPS,
		_pShadowsInputLayout,
		"..\\Models\\Crate_Fragile.obj");

	// add support for shadows on box 1
	m_box1.AddShadowShaders(_pRenderDepthVS,
		_pRenderDepthPS,
		_pRenderDepthInputLayout);

	m_box2.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		1.0f, 
		_pShadowsVS,
		_pShadowsPS,
		_pShadowsInputLayout,
		"..\\Models\\Crate_Fragile.obj");

	// add support for shadows on box 2
	m_box2.AddShadowShaders(_pRenderDepthVS,
		_pRenderDepthPS,
		_pRenderDepthInputLayout);

	// house
	m_house1.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		1.0f, 
		_pShadowsVS,
		_pShadowsPS,
		_pShadowsInputLayout,
		"..\\Models\\house_obj.obj");

	// tree1
	m_tree1.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		1.0f, 
		_pShadowsVS,
		_pShadowsPS,
		_pShadowsInputLayout,
		//"..\\Models\\firtree1.3ds");
		"..\\Models\\Crate_Fragile.obj");

	// tree2
	m_tree2.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		1.0f, 
		_pShadowsVS,
		_pShadowsPS,
		_pShadowsInputLayout,
		//"..\\Models\\firtree1.3ds");
		"..\\Models\\Crate_Fragile.obj");

	// ellipse for bounding world
	m_ellipsoid.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		1.0f, 
		_pSkyVS,
		_pSkyPS,
		_pSkyInputLayout,
		"..\\Models\\ellipsoid.obj");

	D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, "..\\Textures\\grasscube1024.dds", 0, 0, &m_cubeMapSRV, 0);

	// init terrain
	m_terrain.Init(m_pd3dDevice, 
		m_pd3dImmDeviceContext,
		_pTerrainVS,
		_pTerrainPS,
		_pTerrainInputLayout,
		"..\\Textures\\Terrain\\heightmap_1.bmp",
		257,
		257);

	// init water body 1
	m_waterBody1.Init(m_pd3dDevice, 
		m_pd3dImmDeviceContext,
		_pStillWaterVS,
		_pStillWaterPS,
		_pStillWaterInputLayout,
		"", // this will not be using a heightmap
		60,
		60);

	// Set up initial matrices for WVP
	m_matWorld = XMMatrixIdentity();
	m_matProj  = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_nClientWidth / static_cast<float>(m_nClientHeight), 1.0f, 1000.0f);
	m_matView  = XMMatrixLookAtLH(XMVectorSet(-5.0f, 0.0f, -5.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	// light source view matrix
	m_matLightView = XMMatrixIdentity();

	// Update the lights
	m_lightManager.Update(1.0f, m_fmCamera.GetPosition());
}

void TFDemoDriver::OnResize()
{
	TFWinBase::OnResize();
	// Rebuild projection matrix on window resize
	m_matProj = TFMatrixPerspectiveLH(XM_PIDIV4, m_nClientWidth / static_cast<float>(m_nClientHeight), 1.0f, 1000.0f);
}

void TFDemoDriver::Run()
{
	TFCore::TFWinBase::Run();
}

void TFDemoDriver::UpdateScene(float a_fDelta)
{
	TFCore::TFWinBase::UpdateScene(a_fDelta);

	// Update camera (process user input)
	m_fmCamera.Update(a_fDelta);

	// Grab view matrix from the camera
	m_matView = m_fmCamera.GetView();

	// Update the lights
	//m_lightManager.Update(a_fDelta, m_fmCamera.GetPosition());

	if(TFInput::Instance()->IsYPressed())
	{
		XMFLOAT3 _camPos = m_fmCamera.GetPosition();

		stringstream _ss;
		_ss << _camPos.x << ", " << _camPos.y << ", " << _camPos.z << endl;

		OutputDebugStringA(_ss.str().c_str());
	}
}

void TFDemoDriver::RenderToShadowMap()
{
	XMMATRIX _matWVP;

	TFDepthBiasRender(m_pd3dDevice, m_pd3dImmDeviceContext);

	// Set viewport to shadow map viewport, set render target to null (Disables color writes)
	// and set the depth draws to shadow map depth stencil view
	m_pShadowMapFront->SetRenderState();

	// Update the geometry of box 1
	m_matWorld = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	m_matWorld = m_matWorld * XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	
	_matWVP = m_matWorld * m_lightManager.GetView() * m_lightManager.GetProjection();

	// draw box 1
	m_box1.UpdateShadowResources(_matWVP);
	m_box1.ActivateShadowShaders();
	m_box1.Draw();

	// restore default render states
	m_pd3dImmDeviceContext->RSSetState(0);
	m_pd3dImmDeviceContext->OMSetDepthStencilState(0, 0);

	// reset render target
	ResetRenderTarget();
}

void TFDemoDriver::RenderToReflectionMap()
{
	XMMATRIX _matWVP;

	m_pReflectionMap->SetRenderTarget();

	// draw tree 1
	m_matWorld = XMMatrixScaling(0.25f, 0.25f, 0.25f);
	m_matWorld *= XMMatrixTranslation(80, 80, 17);
	_matWVP = m_matWorld * m_matView * m_matProj;

	m_tree1.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_tree1.ActivateShaders();
	m_tree1.BindDefaultTexture();
	m_tree1.Draw();	

	// tree 2
	m_matWorld = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	m_matWorld *= XMMatrixTranslation(70, 60, -17);
	_matWVP = m_matWorld * m_matView * m_matProj;

	m_tree2.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_tree2.ActivateShaders();
	m_tree2.BindDefaultTexture();
	m_tree2.Draw();	

	// draw terrain
	m_matWorld = XMMatrixIdentity();
	_matWVP = m_matWorld * m_matView * m_matProj;


	m_terrain.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_terrain.ActivateShaders();
	m_terrain.Draw();

	// draw skybox
	m_matWorld = XMMatrixScaling(1000.0f, 1000.0f, 1000.0f);
	_matWVP = m_matWorld * m_matView * m_matProj;

	// update render state and depth/stencil state for ellipsoid
	TFRenderNoCull(m_pd3dDevice, m_pd3dImmDeviceContext);
	TFSetDepthLessEqual(m_pd3dDevice, m_pd3dImmDeviceContext);

	m_pd3dImmDeviceContext->PSSetShaderResources(0, 1, &m_cubeMapSRV); // bind cube map SRV
	m_ellipsoid.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_ellipsoid.ActivateShaders();
	m_ellipsoid.Draw();

	// reset render target
	ResetRenderTarget();
}

void TFDemoDriver::RenderScene()
{
	TFCore::TFWinBase::RenderScene();

	// build shadow map
	//RenderToShadowMap();

	RenderToReflectionMap();

	//TFRenderWireframe(m_pd3dDevice, m_pd3dImmDeviceContext);

	// Set world matrix for first box
	/*m_matWorld = XMMatrixScaling(0.2f, 0.2f, 0.2f);*/
	m_matWorld = XMMatrixScaling(1.0f, 1.0f, 1.0f);

	XMFLOAT3 _vLightPos = m_lightManager.GetPosition();

	m_matWorld = m_matWorld * XMMatrixTranslation(_vLightPos.x, 
		_vLightPos.y, 
		_vLightPos.z);

	// Update the geometry with their respective transforms
	XMMATRIX _matWVP = m_matWorld * m_matView * m_matProj;

	m_box1.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_box1.ActivateShaders();
	// bind the shadow map to an input slot of the pixel shader
	//m_box1.SetShadowMap(m_pShadowMapFront->GetDepthMapSRV(), 2);
	m_box1.Draw();


	// draw house 1
	m_matWorld = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	m_matWorld = m_matWorld * XMMatrixRotationAxis(m_fmCamera.GetUpVector(), -XM_PIDIV2);

	m_matWorld = m_matWorld * XMMatrixTranslation(-82.0f, 54.5, 73.0f);
	_matWVP = m_matWorld * m_matView * m_matProj;

	m_house1.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_house1.ActivateShaders();
	m_house1.SetShadowMap(m_pShadowMapFront->GetDepthMapSRV(), 2);
	m_house1.Draw();

	// draw tree 1
	m_matWorld = XMMatrixScaling(0.25f, 0.25f, 0.25f);
	m_matWorld *= XMMatrixTranslation(80, 80, 17);
	_matWVP = m_matWorld * m_matView * m_matProj;

	m_tree1.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_tree1.ActivateShaders();
	//m_tree1.SetShadowMap(m_pShadowMapFront->GetDepthMapSRV(), 2);
	m_tree1.SetReflectionMap(m_pReflectionMap->GetReflectionMapSRV(), 2);
	m_tree1.Draw();	

	// tree 2
	m_matWorld = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	m_matWorld *= XMMatrixTranslation(70, 60, -17);
	_matWVP = m_matWorld * m_matView * m_matProj;

	m_tree2.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_tree2.ActivateShaders();
	//m_tree1.SetShadowMap(m_pShadowMapFront->GetDepthMapSRV(), 2);
	m_tree2.SetReflectionMap(m_pReflectionMap->GetReflectionMapSRV(), 2);
	m_tree2.Draw();	

	// draw terrain
	m_matWorld = XMMatrixIdentity();
	_matWVP = m_matWorld * m_matView * m_matProj;


	m_terrain.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_terrain.ActivateShaders();
	m_terrain.Draw();

	// draw water
	m_matWorld = XMMatrixTranslation(62.0f, 39.0f, -11.0f);
	_matWVP = m_matWorld * m_matView * m_matProj;

	m_waterBody1.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_waterBody1.ActivateShaders();
	m_waterBody1.Draw();

	// draw skybox
	m_matWorld = XMMatrixScaling(1000.0f, 1000.0f, 1000.0f);
	_matWVP = m_matWorld * m_matView * m_matProj;

	// update render state and depth/stencil state for ellipsoid
	TFRenderNoCull(m_pd3dDevice, m_pd3dImmDeviceContext);
	TFSetDepthLessEqual(m_pd3dDevice, m_pd3dImmDeviceContext);

	m_pd3dImmDeviceContext->PSSetShaderResources(0, 1, &m_cubeMapSRV); // bind cube map SRV
	m_ellipsoid.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_ellipsoid.ActivateShaders();
	m_ellipsoid.Draw();

	// restore default states
	m_box1.UnloadShadowMap(2); // unbind shadow maps as shader resources because we are about to rebind them as depth stencil views
	m_pd3dImmDeviceContext->RSSetState(0);
	m_pd3dImmDeviceContext->OMSetDepthStencilState(0, 0);

	// Display the back buffer (vsync intervals)
	//m_pSwapChain->Present( 1, 0 );
	m_pSwapChain->Present(0, 0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, int nCmdShow)
{
	TFDemoDriver _tfApp;
	_tfApp.Init(hInstance, nCmdShow);
	_tfApp.Run();

	return 0;
}
