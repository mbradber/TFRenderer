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
	TFCore::TFCube* m_pGround1;
	TFCore::TFCube* m_pCube1;
	TFCore::TFCube* m_pCube2;
	TFCore::TFFreeMotionCamera m_fmCamera;
	TFCore::TFLightManager m_lightManager;
	TFCore::TFShaderManager m_shaderManager;
	TFCore::TFModel m_model;

	XMMATRIX m_matView;
	XMMATRIX m_matProj;
	XMMATRIX m_matWorld;

	XMMATRIX m_matLightView;

	ID3D11ShaderResourceView* m_cubeMapSRV;
	TFCore::TFModel m_ellipsoid;
	ID3D11Buffer* m_pEllipsoidCB;
	TFRendering::TFShadowMap* m_pShadowMap;

};