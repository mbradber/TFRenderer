#pragma once

#include <d3d11.h>
#include "TFIRenderable.h"

namespace TFCore
{
	class TFCube : public TFIRenderable
	{
	public:
		TFCube();
		TFCube(ID3D11Device* a_pDevice, ID3D11DeviceContext* a_pDeviceContext, float a_fScale);
		~TFCube();

		void Init(ID3D11Device* a_pDevice, 
			ID3D11DeviceContext* a_pDeviceContext, 
			float a_fScale, 
			const std::wstring& a_sFilePath);

		void Draw();

		/*** TFIRenderable methods ***/
		virtual const wchar_t* const GetShaderPath() const;
		virtual const D3D11_INPUT_ELEMENT_DESC* const GetInputLayout() const;
		virtual void SetShaderPath(const std::wstring& a_sFilePath);
		virtual ID3D11VertexShader* GetVertexShader() const; 
		virtual ID3D11PixelShader*  GetPixelShader()  const;

	private:

		virtual void GenerateShaders();

	private:

		ID3D11Device*			 m_pd3dDevice;
		ID3D11DeviceContext*	 m_pDeviceContext;
		ID3D11Buffer*			 m_pVertexBuffer;
		ID3D11Buffer*			 m_pIndexBuffer;
		D3D11_INPUT_ELEMENT_DESC m_aVertexLayout[2];
		ID3D11VertexShader*	     m_pVertexShader;
		ID3D11PixelShader*		 m_pPixelShader;
		ID3D11InputLayout*		 m_pInputLayout;
		float					 m_fScale;
		std::wstring             m_wsShaderPath;
		const size_t			 VERTEX_COUNT;
		const size_t			 INDEX_COUNT;
	};

}



