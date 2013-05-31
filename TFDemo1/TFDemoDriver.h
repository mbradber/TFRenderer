#pragma once

#include "TFCore.h"
#include "TFCube.h"
#include "TFFreeMotionCamera.h"
#include "TFShaderManager.h"
#include "TFLightManager.h"
#include "TFModel.h"
#include "TFShadowMap.h"
#include "TFReflectionMap.h"
#include "TFTerrain.h"
#include "TFWaterStill.h"

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
	void RenderToReflectionMap();

private:

	TFCore::TFFreeMotionCamera m_fmCamera;
	TFCore::TFLightManager m_lightManager;
	TFCore::TFShaderManager m_shaderManager;
	TFCore::TFModel m_box1;
	TFCore::TFModel m_house1;

	XMMATRIX m_matView;
	XMMATRIX m_matProj;
	XMMATRIX m_matWorld;

	XMMATRIX m_matLightView;

	// render to texture views
	TFRendering::TFShadowMap* m_pShadowMapFront;
	TFRendering::TFReflectionMap* m_pReflectionMap;

	// trees
	TFCore::TFModel m_tree1;
	TFCore::TFModel m_tree2;

	// landscape
	TFCore::TFTerrain    m_terrain;
	TFCore::TFWaterStill m_waterBody1;
	ID3D11BlendState*    m_pBlendState1;

	// sky
	TFCore::TFModel m_ellipsoid;
	ID3D11ShaderResourceView* m_cubeMapSRV;
};

