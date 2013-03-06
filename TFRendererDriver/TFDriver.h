#pragma once

#include "TFCore.h"
#include "TFCube.h"
#include "TFFreeMotionCamera.h"

class TFApplication: public TFCore::TFWinBase
{
public:
	TFApplication();
	~TFApplication();

	void Init(HINSTANCE hInstance, int nCmdShow);
	void Run();
	void UpdateScene(float a_fDelta);
	void RenderScene();
	void InitStockShaders();
	void OnResize();

private:
	TFCore::TFIRenderable* m_pCube1;
	TFCore::TFFreeMotionCamera m_fmCamera;

	XMMATRIX m_matView;
	XMMATRIX m_matProj;
	XMMATRIX m_matWorld;
};