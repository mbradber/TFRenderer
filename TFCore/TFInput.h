#pragma once
#include <Windows.h>

namespace TFCore
{
	class TFInput
	{
	public:
		static TFInput* Instance();
		~TFInput();

		void InitializeMouse();
		void SetLeftMouseDown(LPARAM lMouseCoords);
		void SetLeftMouseUp();
		void TrackMouseMove(LPARAM lMouseCoords);
		int GetMouseDeltaX();
		int GetMouseDeltaY();

	private:
		TFInput();
		TFInput(const TFInput&) {}
		TFInput& operator=(const TFInput&) {}
		static TFInput* m_pInstance;

		bool m_bLeftMouseDown;
		int m_nOriginalX;
		int m_nOriginalY;
		int m_nDeltaX;
		int m_nDeltaY;
	};

}