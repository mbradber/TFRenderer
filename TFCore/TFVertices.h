#pragma once

#include "TFTypes.h"

namespace TFCore
{

	const D3D11_INPUT_ELEMENT_DESC TFSimplePositionLayout[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	/*** Simple vertex structure holding position and color ***/
	struct TFSimpleVertex
	{
		tfFloat3 Pos;
		tfFloat4 Col;
	};

	/*** Default input layout for TFSimpleVertex ***/
	const D3D11_INPUT_ELEMENT_DESC TFDefaultSimpleVertexLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	/*** Vertex structure holding position and normals ***/
	struct TFPosNormVertex
	{
		tfFloat3 Pos;
		tfFloat3 Norm;
	};

	/*** Default input layout for TFSimpleVertex ***/
	const D3D11_INPUT_ELEMENT_DESC TFPositionNormalVertexLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	/*** Vertex structure holding position, normals, and texture coordinates ***/
	struct TFPosNormTex
	{
		tfFloat3 Pos;
		tfFloat3 Norm;
		tfFloat2 TexC;
	};

	/*** Vertex structure holding position, normal, and texture coordinate data ***/
	const D3D11_INPUT_ELEMENT_DESC TFPositionNormalTextureLayout[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	struct TFPosNormTexTan
	{
		tfFloat3 Pos;
		tfFloat3 Norm;
		tfFloat2 TexC;
		tfFloat3 TanU;

		TFPosNormTexTan()
			:
			Pos(tfFloat3(0, 0, 0)),
			Norm(tfFloat3(0, 0, 0)),
			TexC(tfFloat2(0, 0)),
			TanU(tfFloat3(0, 0, 0))
		{
		}
	};

	const D3D11_INPUT_ELEMENT_DESC TFPosNormTexTanLayout[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT ,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    2, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	struct TFPosNormTex4Tan
	{
		tfFloat3 Pos;
		tfFloat3 Norm;
		tfFloat4 TexC;
		tfFloat3 TanU;

		TFPosNormTex4Tan()
			:
			Pos(tfFloat3(0, 0, 0)),
			Norm(tfFloat3(0, 0, 0)),
			TexC(tfFloat4(0, 0, 0, 0)),
			TanU(tfFloat3(0, 0, 0))
		{
		}
	};

	const D3D11_INPUT_ELEMENT_DESC TFPosNormTex4TanLayout[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

}