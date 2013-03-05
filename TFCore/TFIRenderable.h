#pragma once

#include <d3d11.h>
#include <string>

namespace TFCore
{
	class TFIRenderable
	{
	public:
		virtual const wchar_t* const GetShaderPath() const = 0;
		virtual const D3D11_INPUT_ELEMENT_DESC* const GetInputLayout() const = 0;
		virtual void SetShaderPath(const std::wstring& a_sFilePath) = 0;
		virtual void Draw() = 0;
		virtual void Init(ID3D11Device* a_pDevice, ID3D11DeviceContext* a_pDeviceContext, float a_fScale, const std::wstring& a_sFilePath) = 0;
		virtual ID3D11VertexShader* GetVertexShader() const = 0;
		virtual ID3D11PixelShader*  GetPixelShader()  const = 0;

		// Renderable object utility function
		void CompileShaderFromFile(const wchar_t* a_cbFileName, LPCSTR a_pEntryPoint, LPCSTR a_pShaderModel, ID3DBlob** a_ppBlobOut);

		virtual ~TFIRenderable() {};

	private:
		virtual void GenerateShaders() = 0;
	};
}