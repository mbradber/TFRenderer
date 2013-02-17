#include "TFCore.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, int nCmdShow)
{
	TFCore::TFWinBase _tfApp;
	_tfApp.InitWindowsApp(hInstance, nCmdShow);
	_tfApp.InitD3D();

	_tfApp.Run();

	return 0;
}