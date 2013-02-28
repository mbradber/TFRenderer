#include "TFInput.h"
#include <sstream>

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
			return m_nDeltaX;
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


}
