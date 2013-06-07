#include "TFInput.h"
#include <sstream>

#define W_KEY 0x57
#define A_KEY 0x41
#define D_KEY 0x44
#define E_KEY 0x45
#define F_KEY 0x46
#define Q_KEY 0x51
#define S_KEY 0x53
#define T_KEY 0x54
#define X_KEY 0x58
#define Y_KEY 0x59
#define Z_KEY 0x5A

namespace TFCore
{
	TFInput* TFInput::m_pInstance = NULL;

	TFInput::TFInput()
	{
	}

	// Init DirectInput
	void TFInput::InitInput(HINSTANCE hi, HWND hWnd)
	{
		DirectInput8Create(hi, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirecInput, NULL);
		//create keyboard device and mouse device
		m_pDirecInput->CreateDevice(GUID_SysKeyboard, &m_pDirectInputDeviceKeyboard, NULL);
		m_pDirecInput->CreateDevice(GUID_SysMouse, &m_pDirectInputDeviceMouse, NULL);
		//set data format for keyboard and mouse
		m_pDirectInputDeviceKeyboard->SetDataFormat(&c_dfDIKeyboard);
		m_pDirectInputDeviceMouse->SetDataFormat(&c_dfDIMouse);
		//set cooperative levels for keyboard and mouse
		m_pDirectInputDeviceKeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		m_pDirectInputDeviceMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	}

	TFInput* TFInput::Instance()
	{
		if(!m_pInstance)
		{
			m_pInstance = new TFInput;
		}

		return m_pInstance;
	}

	TFInput::~TFInput()
	{
		delete m_pInstance;
	}

	void TFInput::DetectInput()
	{
		m_pDirectInputDeviceKeyboard->Acquire();
		m_pDirectInputDeviceMouse->Acquire();

		m_pDirectInputDeviceKeyboard->GetDeviceState(256, (LPVOID)m_keystate);
		m_pDirectInputDeviceMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	}


	int TFInput::GetMouseDeltaX()
	{
		if(m_mouseState.rgbButtons[0] & 0x80)
		{
			return m_mouseState.lX;
		}

		return 0;
	}

	int TFInput::GetMouseDeltaY()
	{
		if(m_mouseState.rgbButtons[0] & 0x80)
		{
			return m_mouseState.lY;
		}

		return 0;
	}

	bool TFInput::IsRightPressed() const
	{
		int _nKeyState = m_keystate[DIK_D] & 0x80;
		return _nKeyState > 0;
	}

	bool TFInput::IsLeftPressed() const
	{
		int _nKeyState = m_keystate[DIK_A] & 0x80;
		return _nKeyState > 0;
	}

	bool TFInput::IsForwardPressed() const
	{
		int _nKeyState = m_keystate[DIK_W] & 0x80;
		return _nKeyState > 0;
	}

	bool TFInput::IsBackPressed() const
	{
		int _nKeyState = m_keystate[DIK_S] & 0x80;
		return _nKeyState > 0;
	}

	bool TFInput::IsUpPressed() const
	{
		int _nKeyState = m_keystate[DIK_E] & 0x80;
		return _nKeyState > 0;
	}

	bool TFInput::IsDownPressed() const
	{
		int _nKeyState = m_keystate[DIK_Q] & 0x80;
		return _nKeyState > 0;
	}

	bool TFInput::IsYPressed() const
	{
		int _nKeyState = m_keystate[DIK_Y] & 0x80;
		return _nKeyState > 0;
	}

	bool TFInput::IsRPressed() const
	{
		int _nKeyState = m_keystate[DIK_R] & 0x80;
		return _nKeyState > 0;
	}

}
