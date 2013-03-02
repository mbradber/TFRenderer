#include "TFDriver.h"
#include "TFMath.h"
#include "TFInput.h"

using namespace TFCore;

void TFApplication::Init(HINSTANCE hInstance, int nCmdShow)
{
	InitWindowsApp(hInstance, nCmdShow);
	InitD3D();

	// Init shader management
	m_shaderManager.Init(m_pd3dDevice, m_pd3dImmDeviceContext);
	// The next two calls generate vertex and pixel shaders from a stock shader file
	// and set the active vertex and pixel shader objects.
	m_shaderManager.GenerateDefaultPosColVertexShader();
	m_shaderManager.GenerateDefaultPosColPixelShader();
	// Generates a constant buffer object that will communicate the WVP matrix between CPU-GPU
	m_shaderManager.GenerateDefaultPosColWVPBuffer();

	// Init geometry
	m_cube1.Init(m_pd3dDevice, m_pd3dImmDeviceContext, 1.0f);

	// Set up initial matrices for WVP
	m_matWorld = TFGetMatrixIdentity();
	m_matProj  = TFMatrixPerspectiveLH(XM_PIDIV4, m_nClientWidth / static_cast<float>(m_nClientHeight), 0.01f, 100.0f);
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

	// Camera updates from keyboard (directional)
	if(TFInput::Instance()->IsRightPressed())
	{
		m_fmCamera.MoveRight(a_fDelta);
	}
	if(TFInput::Instance()->IsLeftPressed())   
	{
		m_fmCamera.MoveLeft(a_fDelta);
	}
	if(TFInput::Instance()->IsForwardPressed())
	{
		m_fmCamera.MoveForward(a_fDelta);
	}
	if(TFInput::Instance()->IsBackPressed())
	{
		m_fmCamera.MoveBack(a_fDelta);
	}

	// Camera updates from mouse (rotations)
	m_fmCamera.RotateCameraYaw(a_fDelta, static_cast<float>(TFInput::Instance()->GetMouseDeltaX()));

	// Grab view matrix from the camera
	m_matView = m_fmCamera.GetView();
}

void TFApplication::RenderScene()
{
	TFCore::TFWinBase::RenderScene();

	// Set wireframe/no cull mode for debug
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = false;
	rd.DepthClipEnable = true;

	ID3D11RasterizerState* _pRasterizerState;
	m_pd3dDevice->CreateRasterizerState(&rd, &_pRasterizerState);
	m_pd3dImmDeviceContext->RSSetState(_pRasterizerState);

	// Update WVP constant buffer
	TFCore::TFBufferWVP cb;

	//cb.world      = TFMatrixTranspose(m_matWorld);
	//cb.view       = TFMatrixTranspose(m_matView);
	//cb.projection = TFMatrixTranspose(m_matProj);

	XMMATRIX _matWVP = XMMatrixMultiply(m_matWorld, m_matView);
	_matWVP = XMMatrixMultiply(_matWVP, m_matProj);
	cb.wvpMatrix = TFMatrixTranspose(_matWVP);


	ID3D11Buffer* _pConstantBuffer = m_shaderManager.GetWVPBuffer();
	m_pd3dImmDeviceContext->UpdateSubresource(_pConstantBuffer, 0, NULL, &cb, 0, 0);
	
	// Render geometry
	m_pd3dImmDeviceContext->VSSetShader(m_shaderManager.GetActiveVertexShader(), NULL, 0);
	m_pd3dImmDeviceContext->PSSetShader(m_shaderManager.GetActivePixelShader(), NULL, 0);
	m_pd3dImmDeviceContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);

	m_cube1.Draw();

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