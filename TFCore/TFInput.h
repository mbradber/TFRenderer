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
		void SetKeyDown(WPARAM wKeyPress);
		void SetKeyUp(WPARAM wKeyRelease);
		void TrackMouseMove(LPARAM lMouseCoords);
		int GetMouseDeltaX();
		int GetMouseDeltaY();
		bool IsRightPressed()   const;
		bool IsLeftPressed()    const;
		bool IsForwardPressed() const;
		bool IsBackPressed()    const;
		bool IsDownPressed()    const;
		bool IsUpPressed()      const;
		bool IsYPressed()       const;

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

		bool m_aKeyDown[256];
	};

}