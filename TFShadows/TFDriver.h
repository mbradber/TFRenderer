#pragma once

#include "TFCore.h"
#include "TFCube.h"
#include "TFFreeMotionCamera.h"
#include "TFShaderManager.h"
#include "TFLightManager.h"
#include "TFModel.h"
#include "TFShadowMap.h"

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
	void RenderToShadowMap();

private:

	TFCore::TFFreeMotionCamera m_fmCamera;
	TFCore::TFLightManager m_lightManager;
	TFCore::TFShaderManager m_shaderManager;
	TFCore::TFModel m_box1;
	TFCore::TFModel m_box2;

	tfMatrix m_matView;
	tfMatrix m_matProj;
	tfMatrix m_matWorld;

	tfMatrix m_matLightView;

	TFRendering::TFShadowMap* m_pShadowMapFront;
	TFRendering::TFShadowMap* m_pShadowMapBack;

};