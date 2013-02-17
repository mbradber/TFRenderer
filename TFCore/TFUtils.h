#pragma once

#include <iostream>
#include <Windows.h>
#include <D3DX11.h>
#include <DxErr.h>

#define LINE_NO   __LINE__
#define FILE_NAME __FILE__


#ifdef  _DEBUG

inline void TF_ASSERT(bool a_bCondition, char* a_cbFileName, size_t a_nLineNum)
{
	if(!a_bCondition)
	{
		char _debugString [1000];                         
		sprintf_s(_debugString, 1000 * sizeof(char), "ASSERTION FAILURE!\nFile:%s\nLine:%d\n", a_cbFileName, a_nLineNum); 
		MessageBoxA(0, _debugString, "TreeFrog Assertion Failure", MB_OK); 
		exit(EXIT_FAILURE);
	}
}

#else
inline void TF_ASSERT(bool a_bCondition, char* a_cbFileName, size_t a_nLineNum)
{}
#endif 

//---------------------------------------------------------------------------------------
// Simple d3d error checker
//---------------------------------------------------------------------------------------

#if defined(DEBUG) || defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
	{                                                      \
	HRESULT hr = (x);                                      \
	if(FAILED(hr))                                         \
		{                                                  \
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
		}                                                  \
	}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 


//---------------------------------------------------------------------------------------
// Convenience macro for releasing COM objects.
//---------------------------------------------------------------------------------------

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }