#pragma once

#include <iostream>
#include <Windows.h>
#include <D3DX11.h>
#include <DxErr.h>
#include <tchar.h>

//---------------------------------------------------------------------------------------
// Simple assertion for TreeFrog Engine
//---------------------------------------------------------------------------------------

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

//---------------------------------------------------------------------------------------
// HRESULT interpreter from MSDN
//---------------------------------------------------------------------------------------

inline void TFErrorDescription(HRESULT hr) 
{ 
     if(FACILITY_WINDOWS == HRESULT_FACILITY(hr)) 
         hr = HRESULT_CODE(hr); 
     TCHAR* szErrMsg; 

     if(FormatMessage( 
       FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, 
       NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
       (LPTSTR)&szErrMsg, 0, NULL) != 0) 
     { 
		 OutputDebugString(szErrMsg);
         LocalFree(szErrMsg); 
     } else 
		OutputDebugStringA("[Could not find a description for error.]");
}