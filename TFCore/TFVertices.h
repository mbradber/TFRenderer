#pragma once

#include "TFTypes.h"

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
    { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};