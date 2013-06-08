#pragma once
#include "TFEffect.h"

namespace TFRendering
{

	class TFBlinnPhong : public TFEffect
	{
	public:
		TFBlinnPhong();
		~TFBlinnPhong();

		void BatchDraw();
	};

}