#pragma once

#include "TFCore.h"
#include "TFCube.h"
#include "TFFreeMotionCamera.h"
#include "TFShaderManager.h"
#include "TFLightManager.h"
#include "TFEffect.h"
#include "TFIRenderable.h"
#include "TFBlinnPhong.h"

class TFDemo2Driver : public TFCore::TFWinBase
{
public:
	TFDemo2Driver(void);
	~TFDemo2Driver(void);

	void Init(HINSTANCE hInstance, int a_nCmdShow);
	void Run();
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

	tfMatrix m_matLightView;

	// render to texture views
	//TFRendering::TFShadowMap* m_pShadowMapFront;
	//TFRendering::TFReflectionMap* m_pReflectionMap;

	//// landscape
	//TFCore::TFTerrain    m_terrain;
	//TFCore::TFWaterStill m_waterBody1;
	//TFCore::TFWaterStill m_waterBody2;
	//ID3D11BlendState*    m_pBlendState1;

	//// sky
	//TFCore::TFModel m_ellipsoid;
	//ID3D11ShaderResourceView* m_cubeMapSRV;


	// Effects
	TFRendering::TFEffect* m_pEffect1;
	TFRendering::TFIRenderable* m_pModelEx1;
};

