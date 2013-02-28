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
#define Z_KEY 0x5A

namespace TFCore
{
	TFInput* TFInput::m_pInstance = NULL;

	TFInput::TFInput()
		:m_bLeftMouseDown(false),
		 m_nOriginalX(0),
		 m_nOriginalY(0),
		 m_nDeltaX(0),
		 m_nDeltaY(0)
	{
		memset(m_aKeyDown, 0, 256 * sizeof(bool));
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

	void TFInput::InitializeMouse()
	{
		OutputDebugString(L"Mouse init\n");
	}

	void TFInput::SetLeftMouseDown(LPARAM lMouseCoords)
	{
		POINTS _points = MAKEPOINTS(lMouseCoords);
		m_nOriginalX = _points.x;
		m_nOriginalY = _points.y;

		std::stringstream _ss;
		_ss << "Mouse pos clicked at X:" << m_nOriginalX << ", Y:" << m_nOriginalY << std::endl;
		OutputDebugStringA(_ss.str().c_str());

		m_bLeftMouseDown = true;
	}

	void TFInput::SetLeftMouseUp()
	{
		OutputDebugString(L"Left mouse RELEASED.\n");
		m_bLeftMouseDown = false;
	}

	void TFInput::TrackMouseMove(LPARAM lMouseCoords)
	{
		if(m_bLeftMouseDown)
		{
			POINTS _points = MAKEPOINTS(lMouseCoords);
			m_nDeltaX = _points.x - m_nOriginalX;
			m_nDeltaY = _points.y - m_nOriginalY;

			std::stringstream _ss;
			_ss << "Mouse delta X:" << m_nDeltaX << ", Y:" << m_nDeltaY << std::endl;

			OutputDebugStringA(_ss.str().c_str());
		}
	}

	int TFInput::GetMouseDeltaX()
	{
		if(m_bLeftMouseDown)
		{
			// store the delta since last check
			int _delta    = m_nDeltaX; 
			// Update the original position to the current position and reset the delta field
			m_nOriginalX += m_nDeltaX; 
			m_nDeltaX     = 0;

			return _delta;
		}

		return 0;
	}

	int TFInput::GetMouseDeltaY()
	{
		if(m_bLeftMouseDown)
		{
			return m_nDeltaY;
		}

		return 0;
	}

	void TFInput::SetKeyDown(WPARAM wKeyPress)
	{
		OutputDebugString(L"Key down\n");
		m_aKeyDown[wKeyPress] = true;
	}

	void TFInput::SetKeyUp(WPARAM wKeyPress)
	{
		OutputDebugStringA("Key up\n");
		m_aKeyDown[wKeyPress] = false;
	}

	bool TFInput::IsRightPressed()
	{
		return m_aKeyDown[D_KEY];
	}

	bool TFInput::IsLeftPressed()
	{
		return m_aKeyDown[A_KEY];
	}

	bool TFInput::IsForwardPressed()
	{
		return m_aKeyDown[W_KEY];
	}

	bool TFInput::IsBackPressed()
	{
		return m_aKeyDown[S_KEY];
	}

}
