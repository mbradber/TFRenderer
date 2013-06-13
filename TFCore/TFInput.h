#pragma once

/*** Input singleton class. Uses DirectInput to manage the state of mouse
	 and keyboard ***/

#include <Windows.h>
#include <dinput.h>

namespace TFCore
{
	class TFInput
	{
	public:
		static TFInput* Instance();
		~TFInput();

		int GetMouseDeltaX();
		int GetMouseDeltaY();
		bool IsRightPressed()   const;
		bool IsLeftPressed()    const;
		bool IsForwardPressed() const;
		bool IsBackPressed()    const;
		bool IsDownPressed()    const;
		bool IsUpPressed()      const;
		bool IsYPressed()       const;
		bool IsRPressed()       const;
		void DetectInput();
		void InitInput(HINSTANCE hi, HWND hWnd);

	private:
		TFInput();
		TFInput(const TFInput&) {}
		TFInput& operator=(const TFInput&) {}
		static TFInput* m_pInstance;

		// Direct Input vars
		LPDIRECTINPUT8 m_pDirecInput;
		LPDIRECTINPUTDEVICE8 m_pDirectInputDeviceKeyboard;
		LPDIRECTINPUTDEVICE8 m_pDirectInputDeviceMouse;
		BYTE m_keystate[256];
		DIMOUSESTATE m_mouseState;
	};

}