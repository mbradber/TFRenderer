#pragma once

#include "TFCore.h"
#include "TFCube.h"
#include "TFFreeMotionCamera.h"
#include "TFShaderManager.h"
#include "TFLightManager.h"
#include "TFModel.h"
#include "TFShadowMap.h"
#include "TFTerrain.h"

class TFDemoDriver : public TFCore::TFWinBase
{
public:
	TFDemoDriver(void);
	~TFDemoDriver(void);

	void Init(HINSTANCE hInstance, int a_nCmdShow);
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
	TFCore::TFModel m_house1;

	XMMATRIX m_matView;
	XMMATRIX m_matProj;
	XMMATRIX m_matWorld;

	XMMATRIX m_matLightView;

	TFRendering::TFShadowMap* m_pShadowMapFront;

	TFCore::TFTerrain m_terrain;
};

