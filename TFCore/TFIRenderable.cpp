#include "TFIRenderable.h"
#include <d3dcompiler.h>
#include "TFUtils.h"

namespace TFCore
{
	void TFIRenderable::CompileShaderFromFile(const wchar_t* a_cbFileName, LPCSTR a_pEntryPoint, LPCSTR a_pShaderModel, ID3DBlob** a_ppBlobOut)
	{
		DWORD _dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
		_dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

		ID3DBlob* _pErrorBlob = NULL;
		HRESULT _hr = D3DX11CompileFromFile(a_cbFileName, NULL, NULL, a_pEntryPoint, a_pShaderModel, 
			_dwShaderFlags, 0, NULL, a_ppBlobOut, &_pErrorBlob, NULL);

		// Output debug info if compilation failed
		if(FAILED(_hr))
		{
			if(_pErrorBlob != NULL)
			{
				OutputDebugStringA((char*)_pErrorBlob->GetBufferPointer());
			}

			HR(_hr);
		}

		ReleaseCOM(_pErrorBlob);
	}
}