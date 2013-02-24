#include "TFDriver.h"

void TFApplication::Init(HINSTANCE hInstance, int nCmdShow)
{
	InitWindowsApp(hInstance, nCmdShow);
	InitD3D();

	// Init shader management
	m_shaderManager.Init(m_pd3dDevice);

	// Init geometry
	m_cube1.Init(m_pd3dDevice, m_pd3dImmDeviceContext, 5.0f);

	// Acquire vertex shader
}

void TFApplication::Run()
{

}

void TFApplication::UpdateScene()
{

}

void TFApplication::RenderScene()
{

}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, int nCmdShow)
{
	TFApplication _tfApp;
	_tfApp.Init(hInstance, nCmdShow);
	_tfApp.Run();

	return 0;
}