#include "TFDriver.h"




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, int nCmdShow)
{
	TFApplication _tfApp;
	_tfApp.InitWindowsApp(hInstance, nCmdShow);
	_tfApp.InitD3D();

	_tfApp.Run();

	return 0;
}