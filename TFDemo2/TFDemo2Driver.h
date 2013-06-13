#pragma once

#include "TFCore.h"
#include "TFCube.h"
#include "TFFreeMotionCamera.h"
#include "TFShaderManager.h"
#include "TFLightManager.h"
#include "TFIEffect.h"
#include "TFIRenderable.h"
#include "TFShadowMap.h"
#include "TFReflectionMap.h"

class TFDemo2Driver : public TFCore::TFWinBase
{
public:
	TFDemo2Driver();
	~TFDemo2Driver();

	void Init(HINSTANCE hInstance, int a_nCmdShow);
	void UpdateScene(float a_fDelta);
	void RenderScene();
	void InitStockShaders();
	void OnResize();
	void RenderToShadowMap();
	void RenderToReflectionMap();

private:

	TFCore::TFFreeMotionCamera m_fmCamera;
	TFCore::TFLightManager m_lightManager;
	TFCore::TFShaderManager m_shaderManager;

	tfMatrix m_matView;
	tfMatrix m_matProj;
	tfMatrix m_matWorld;

	// render to texture views
	TFRendering::TFShadowMap* m_pShadowMapFront;
	TFRendering::TFReflectionMap* m_pReflectionMap;

	// Effects
	TFRendering::TFIEffect* m_pBlinnPhongFX;
	TFRendering::TFIEffect* m_pTerrainFX;
	TFRendering::TFIEffect* m_pRenderDepthFX;
	TFRendering::TFIEffect* m_pWaterStillFX;
	TFRendering::TFIEffect* m_pSkyboxFX;
	TFRendering::TFIEffect* m_pFoliageFX;

	// Renderables
	TFRendering::TFIRenderable* m_pHouseModel;
	TFRendering::TFIRenderable* m_pTree1;
	TFRendering::TFIRenderable* m_pTree2;
	TFRendering::TFIRenderable* m_pTree3;
	TFRendering::TFIRenderable* m_pTerrain;
	TFRendering::TFIRenderable* m_pWater1;
	TFRendering::TFIRenderable* m_pWater2;
	TFRendering::TFIRenderable* m_pSkybox;
	TFRendering::TFIRenderable* m_pGnome;
	TFRendering::TFIRenderable* m_pDoor1;

};

