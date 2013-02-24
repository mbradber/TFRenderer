#pragma once

#include "TFCore.h"
#include "TFCube.h"
#include "TFShaderManager.h"

class TFApplication: public TFCore::TFWinBase
{
public:
	void Init(HINSTANCE hInstance, int nCmdShow);
	void Run();
	void UpdateScene(float a_fDelta);
	void RenderScene();
private:
	TFCore::TFCube m_cube1;
	TFCore::TFShaderManager m_shaderManager;

	tfMatrix m_matView;
	tfMatrix m_matProj;
	tfMatrix m_matWorld;
};