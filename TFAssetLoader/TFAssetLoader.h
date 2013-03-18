#pragma once

#include "TFCore.h"
#include "TFCube.h"
#include "TFFreeMotionCamera.h"
#include "TFShaderManager.h"
#include "TFLightManager.h"
#include "TFModel.h"

class TFApplication: public TFCore::TFWinBase
{
public:
	TFApplication();
	~TFApplication();

	void Init(HINSTANCE hInstance, int nCmdShow);
	void Run();
	void UpdateScene(float a_fDelta);
	void RenderScene();
	void OnResize();

private:

	TFCore::TFFreeMotionCamera m_fmCamera;
	TFCore::TFLightManager m_lightManager;
	TFCore::TFShaderManager m_shaderManager;
	TFCore::TFModel m_spider;
	TFCore::TFModel m_terrain;
	TFCore::TFModel m_tree1;
	TFCore::TFModel m_dwarf1;
	TFCore::TFModel m_zard1;

	XMMATRIX m_matView;
	XMMATRIX m_matProj;
	XMMATRIX m_matWorld;
};