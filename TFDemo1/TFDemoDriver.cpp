#include "TFDemoDriver.h"
#include "TFMath.h"
#include "TFInput.h"
#include "TFVertices.h"
#include "TFUtils.h"
#include "TFModel.h"
#include <sstream>
#include <string>

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
	
#ifdef _DEBUG
	wstring _wsShaderPrefix = L"..\\Debug\\";

#else
	wstring _wsShaderPrefix = L"..\\Release\\";
#endif

	// register shaders 
	// TODO: don't always use the debug shaders...
	m_shaderManager.SetShaderPrefix(_wsShaderPrefix);

	m_shaderManager.AddVertexShader(L"RenderDepth", L"RenderDepthVS.cso", TFPosNormTexTanLayout, 4);
	m_shaderManager.AddPixelShader(L"RenderDepth",  L"RenderDepthPS.cso");

	m_shaderManager.AddVertexShader(L"Shadows", L"ShadowsVS.cso", TFPosNormTexTanLayout, 4);
	m_shaderManager.AddPixelShader(L"Shadows",  L"ShadowsPS.cso");

	m_shaderManager.AddVertexShader(L"Terrain", L"TerrainVS.cso", TFPosNormTex4TanLayout, 4);
	m_shaderManager.AddPixelShader(L"Terrain",  L"TerrainPS.cso");

	m_shaderManager.AddVertexShader(L"StillWater", L"StillWaterVS.cso", TFPosNormTex4TanLayout, 4);
	m_shaderManager.AddPixelShader(L"StillWater",  L"StillWaterPS.cso");

	m_shaderManager.AddVertexShader(L"Sky", L"SkyVS.cso", TFPosNormTexTanLayout, 4);
	m_shaderManager.AddPixelShader(L"Sky",  L"SkyPS.cso");

	m_shaderManager.AddVertexShader(L"Foliage", L"FoliageVS.cso", TFPosNormTexTanLayout, 4);
	m_shaderManager.AddPixelShader(L"Foliage",  L"FoliagePS.cso");

	// bind samplers
	ID3D11SamplerState* _defaultSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_ANISOTROPIC);
	m_pd3dImmDeviceContext->PSSetSamplers(0, 1, &_defaultSampler);
	_defaultSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_TRILINEAR);
	m_pd3dImmDeviceContext->PSSetSamplers(1, 1, &_defaultSampler);
	_defaultSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_SHADOW);
	m_pd3dImmDeviceContext->PSSetSamplers(2, 1, &_defaultSampler);
	_defaultSampler = m_shaderManager.GetSamplerState(TF_SAMPLER_POINT);
	m_pd3dImmDeviceContext->PSSetSamplers(3, 1, &_defaultSampler);

	// blend states
	D3D11_BLEND_DESC blendDesc1 = {0};
	blendDesc1.AlphaToCoverageEnable = false;
	blendDesc1.IndependentBlendEnable = false;

	blendDesc1.RenderTarget[0].BlendEnable = true;
	blendDesc1.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc1.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc1.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc1.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc1.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc1.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc1.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_pd3dDevice->CreateBlendState(&blendDesc1, &m_pBlendState1);

	// get handles to shaders and input layouts

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

	ID3D11VertexShader* _pFoliageVS              = m_shaderManager.GetVertexShaderByName(L"Foliage");
	ID3D11PixelShader*  _pFoliagePS              = m_shaderManager.GetPixelShaderByName( L"Foliage");
	ID3D11InputLayout*  _pFoliageInputLayout     = m_shaderManager.GetInputLayoutByName( L"Foliage");

	// create render to texture maps
	m_pShadowMapFront = new TFRendering::TFShadowMap(m_pd3dDevice, m_pd3dImmDeviceContext, 2048, 2048);
	m_pReflectionMap  = new TFRendering::TFReflectionMap(m_pd3dDevice, m_pd3dImmDeviceContext, 512, 512);

	XMMATRIX _matWorld = XMMatrixIdentity();

	// init models
	m_box1.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		_pShadowsVS,
		_pShadowsPS,
		_pShadowsInputLayout,
		"..\\Models\\Crate_Fragile.obj");

	// add support for shadows on box 1
	m_box1.AddShadowShaders(_pRenderDepthVS,
		_pRenderDepthPS,
		_pRenderDepthInputLayout);

	// house
	m_house1.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		_pShadowsVS,
		_pShadowsPS,
		_pShadowsInputLayout,
		"..\\Models\\house_obj.obj");

	_matWorld  = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	_matWorld *= XMMatrixRotationAxis(m_fmCamera.GetUpVector(), XM_PIDIV2);
	_matWorld *= XMMatrixTranslation(79.0f, 54.5, -70.0f);

	m_house1.SetWorldMatrix(_matWorld);

	// add support for shadows on terrain
	m_house1.AddShadowShaders(_pRenderDepthVS,
		_pRenderDepthPS,
		_pRenderDepthInputLayout);

	// tree1
	m_tree1.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		_pFoliageVS,
		_pFoliagePS,
		_pFoliageInputLayout,
		"..\\Models\\palm4.obj");

	_matWorld  = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	_matWorld *= XMMatrixRotationAxis(m_fmCamera.GetUpVector(), -XM_PIDIV2 + XM_PIDIV4);
	//_matWorld *= XMMatrixTranslation(-55, 43, 35);
	_matWorld *= XMMatrixTranslation(-51.0f, 42.0f, -14.0f);

	m_tree1.SetWorldMatrix(_matWorld);

	// add support for shadows on tree 1
	m_tree1.AddShadowShaders(_pRenderDepthVS,
		_pRenderDepthPS,
		_pRenderDepthInputLayout);

	// tree2
	m_tree2.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
		_pFoliageVS,
		_pFoliagePS,
		_pFoliageInputLayout,
		"..\\Models\\palm3.obj");

	_matWorld  = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	_matWorld *= XMMatrixRotationAxis(m_fmCamera.GetUpVector(), -XM_PIDIV2);
	//_matWorld *= XMMatrixTranslation(-51, 41.5, -12);
	_matWorld *= XMMatrixTranslation(-55, 43, 35);

	m_tree2.SetWorldMatrix(_matWorld);

	// add support for shadows on tree 1
	m_tree2.AddShadowShaders(_pRenderDepthVS,
		_pRenderDepthPS,
		_pRenderDepthInputLayout);

	// ellipse for bounding world
	m_ellipsoid.Init(m_pd3dDevice,
		m_pd3dImmDeviceContext, 
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

	_matWorld = XMMatrixRotationAxis(m_fmCamera.GetUpVector(), -XM_PI);
	m_terrain.SetWorldMatrix(_matWorld);

	// add support for shadows on terrain
	m_terrain.AddShadowShaders(_pRenderDepthVS,
		_pRenderDepthPS,
		_pRenderDepthInputLayout);

	// init water body 1
	m_waterBody1.Init(m_pd3dDevice, 
		m_pd3dImmDeviceContext,
		_pStillWaterVS,
		_pStillWaterPS,
		_pStillWaterInputLayout,
		"", // this will not be using a heightmap
		50,
		50);

	_matWorld = XMMatrixTranslation(-58.0f, 39.0f, 8.0f);
	m_waterBody1.SetWorldMatrix(_matWorld);

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

	// Update water sim
	m_waterBody1.Update(a_fDelta);

	if(TFInput::Instance()->IsYPressed())
	{
		XMFLOAT3 _camPos = m_fmCamera.GetPosition();
		//XMFLOAT4 _camPos;
		//XMVECTOR _vCamDir = XMVector4Normalize(m_fmCamera.GetForward());
		//XMStoreFloat4(&_camPos, _vCamDir);

		stringstream _ss;
		_ss << _camPos.x << ", " << _camPos.y << ", " << _camPos.z << endl;

		OutputDebugStringA(_ss.str().c_str());
	}
}

void TFDemoDriver::RenderToShadowMap()
{
	XMMATRIX _matWVP;
	XMMATRIX _matViewProj = m_lightManager.GetView() * m_lightManager.GetProjection();

	TFDepthBiasRender(m_pd3dDevice, m_pd3dImmDeviceContext);

	// Set viewport to shadow map viewport, set render target to null (Disables color writes)
	// and set the depth draws to shadow map depth stencil view
	m_pShadowMapFront->SetRenderState();

	/*** HOUSE ***/
	m_matWorld = m_house1.GetWorldMatrix();
	_matWVP = m_matWorld * _matViewProj;

	m_house1.UpdateShadowResources(_matWVP);
	m_house1.ActivateShadowShaders();
	m_house1.Draw();


	/*** TREE 1 ***/
	m_matWorld = m_tree1.GetWorldMatrix();
	_matWVP = m_matWorld * _matViewProj;

	m_tree1.UpdateShadowResources(_matWVP);
	m_tree1.ActivateShadowShaders();
	m_tree1.Draw();	

	/*** TREE 2 ***/
	m_matWorld = m_tree2.GetWorldMatrix();
	_matWVP = m_matWorld * _matViewProj;

	m_tree2.UpdateShadowResources(_matWVP);
	m_tree2.ActivateShadowShaders();
	m_tree2.Draw();	


	/*** TERRAIN ***/
	//XMFLOAT4 _f4ClipData;
	//_f4ClipData.x = 0.0f; // height of this reflective surface in world space (set to 0 since not using)
	//_f4ClipData.y = 0.0f;  // whether or not to use a clip plane in the VS
	////m_terrain.UpdateFrameData(_f4ClipData);

	//m_matWorld = m_terrain.GetWorldMatrix();
	//_matWVP = m_matWorld * _matViewProj;

	//m_terrain.UpdateShadowResources(_matWVP);
	//m_terrain.ActivateShadowShaders();
	//m_terrain.Draw();

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

	// compute view-proj matrix
	XMMATRIX _matViewProj = m_matView * m_matProj;

	TFRenderFrontFaceCull(m_pd3dDevice, m_pd3dImmDeviceContext);

	XMMATRIX _matFlip = XMMatrixScaling(1.0f, -1.0f, 1.0f);
	float _fPlaneVerticalOffset = 39.0f;

	/*** TREE 1 ***/
	m_matWorld = m_tree1.GetWorldMatrix();
	m_matWorld *= _matFlip; // flip the object
	m_matWorld *= XMMatrixTranslation(0, 2 * _fPlaneVerticalOffset, 0);
	_matWVP = m_matWorld * _matViewProj;

	m_tree1.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_tree1.ActivateShaders();
	m_tree1.Draw();	

	/*** TREE 2 ***/
	m_matWorld = m_tree2.GetWorldMatrix();
	m_matWorld *= _matFlip; // flip the object
	m_matWorld *= XMMatrixTranslation(0, 2 * _fPlaneVerticalOffset, 0); // move the object up by 2 * reflection plane offset...
	_matWVP = m_matWorld * _matViewProj;

	m_tree2.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_tree2.ActivateShaders();
	m_tree2.Draw();	

	/*** SKY ***/
	m_matWorld = XMMatrixScaling(1000.0f, 1000.0f, 1000.0f);
	m_matWorld *= _matFlip; // flip the object
	m_matWorld *= XMMatrixTranslation(0, 2 * _fPlaneVerticalOffset, 0); // move the object up by 2 * reflection plane offset...
	_matWVP = m_matWorld * _matViewProj;

	// update render state and depth/stencil state for ellipsoid
	TFRenderNoCull(m_pd3dDevice, m_pd3dImmDeviceContext);
	TFSetDepthLessEqual(m_pd3dDevice, m_pd3dImmDeviceContext);

	m_pd3dImmDeviceContext->PSSetShaderResources(0, 1, &m_cubeMapSRV); // bind cube map SRV
	m_ellipsoid.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_ellipsoid.ActivateShaders();
	m_ellipsoid.Draw();


	/*** TERRAIN ***/
	XMFLOAT4 _f4ClipData;
	_f4ClipData.x = 39.0f; // height of this reflective surface in world space
	_f4ClipData.y = 1.0f;  // whether or not to use a clip plane in the VS
	m_terrain.UpdateFrameData(_f4ClipData);

	m_matWorld = m_terrain.GetWorldMatrix();
	m_matWorld *= _matFlip; // flip the object
	m_matWorld *= XMMatrixTranslation(0, 2 * _fPlaneVerticalOffset, 0); // move the object up by 2 * reflection plane offset...
	_matWVP = m_matWorld * _matViewProj;

	m_terrain.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_terrain.ActivateShaders();
	m_terrain.Draw();


	m_pd3dImmDeviceContext->RSSetState(0);


	// reset render target
	ResetRenderTarget();
}

void TFDemoDriver::RenderScene()
{
	TFCore::TFWinBase::RenderScene();

	// build shadow map
	RenderToShadowMap();

	RenderToReflectionMap();

	//TFRenderWireframe(m_pd3dDevice, m_pd3dImmDeviceContext);

	// compute view-proj matrix
	XMMATRIX _matViewProj = m_matView * m_matProj;
	XMMATRIX _matWVP = XMMatrixIdentity();

	/*** BOX 1 (light source) ***/
	m_matWorld = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMFLOAT3 _vLightPos = m_lightManager.GetPosition();

	m_matWorld = m_matWorld * XMMatrixTranslation(_vLightPos.x, 
		_vLightPos.y, 
		_vLightPos.z);

	_matWVP = m_matWorld * _matViewProj;

	m_box1.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_box1.ActivateShaders();
	// bind the shadow map to an input slot of the pixel shader
	m_box1.SetShadowMap(m_pShadowMapFront->GetDepthMapSRV(), 2);
	m_box1.Draw();

	/*** HOUSE ***/
	m_matWorld = m_house1.GetWorldMatrix();
	_matWVP = m_matWorld * _matViewProj;

	m_house1.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_house1.ActivateShaders();
	m_house1.SetShadowMap(m_pShadowMapFront->GetDepthMapSRV(), 2);
	m_house1.Draw();

	/*** TREE 1 ***/
	m_matWorld = m_tree1.GetWorldMatrix();
	_matWVP = m_matWorld * _matViewProj;

	m_tree1.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_tree1.ActivateShaders();
	//m_tree1.SetShadowMap(m_pShadowMapFront->GetDepthMapSRV(), 2);
	m_tree1.SetReflectionMap(m_pReflectionMap->GetReflectionMapSRV(), 2);
	m_tree1.Draw();	

	/*** TREE 2 ***/
	m_matWorld = m_tree2.GetWorldMatrix();
	_matWVP = m_matWorld * _matViewProj;

	m_tree2.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_tree2.ActivateShaders();
	//m_tree1.SetShadowMap(m_pShadowMapFront->GetDepthMapSRV(), 2);
	m_tree2.SetReflectionMap(m_pReflectionMap->GetReflectionMapSRV(), 2);
	m_tree2.Draw();	


	/*** TERRAIN ***/
	XMFLOAT4 _f4ClipData;
	_f4ClipData.x = 0.0f; // height of this reflective surface in world space (set to 0 since not using)
	_f4ClipData.y = 0.0f;  // whether or not to use a clip plane in the VS
	m_terrain.UpdateFrameData(_f4ClipData);

	m_matWorld = m_terrain.GetWorldMatrix();
	_matWVP = m_matWorld * _matViewProj;

	m_terrain.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_terrain.ActivateShaders();
	m_terrain.SetShadowMap(m_pShadowMapFront->GetDepthMapSRV(), 6); // bind shadow map
	m_terrain.Draw();

	/*** WATER ***/
	m_matWorld = m_waterBody1.GetWorldMatrix();
	_matWVP = m_matWorld * _matViewProj;

	m_waterBody1.UpdateResources(_matWVP, m_matWorld, m_waterBody1.GetTextureTransformNeg(), m_waterBody1.GetTextureTransform(), m_fmCamera.GetPosition());
	m_waterBody1.BindReflectionMap(m_pReflectionMap->GetReflectionMapSRV());
	m_waterBody1.ActivateShaders();

	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pd3dImmDeviceContext->OMSetBlendState(m_pBlendState1, blendFactors, 0xffffffff);

	m_waterBody1.Draw();

	m_pd3dImmDeviceContext->OMSetBlendState(NULL, blendFactors, 0xffffffff); // reset blend state

	/*** SKY ***/
	m_matWorld = XMMatrixScaling(1000.0f, 1000.0f, 1000.0f);
	_matWVP = m_matWorld * _matViewProj;

	// update render state and depth/stencil state for ellipsoid
	TFRenderNoCull(m_pd3dDevice, m_pd3dImmDeviceContext);
	TFSetDepthLessEqual(m_pd3dDevice, m_pd3dImmDeviceContext);

	m_pd3dImmDeviceContext->PSSetShaderResources(0, 1, &m_cubeMapSRV); // bind cube map SRV
	m_ellipsoid.UpdateResources(_matWVP, m_matWorld, m_matWorld * m_lightManager.GetVPT(), XMMatrixIdentity(), m_fmCamera.GetPosition());
	m_ellipsoid.ActivateShaders();
	m_ellipsoid.Draw();

	// restore default states
	m_box1.UnloadShadowMap(2); // unbind shadow maps as shader resources because we are about to rebind them as depth stencil views
	m_terrain.UnloadShadowMap(6);
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
