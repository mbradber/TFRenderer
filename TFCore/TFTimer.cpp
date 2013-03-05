#include "TFTimer.h"

namespace TFCore
{

	TFTimer::TFTimer()
		:m_nCountsPerSecond(0),
		 m_nInitialTimeCount(0),
		 m_nElapsedGameCount(0),
		 m_nElapsedFrameCount(0)
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&m_nCountsPerSecond);
		QueryPerformanceCounter((LARGE_INTEGER*)&m_nInitialTimeCount);
	}


	TFTimer::~TFTimer(void)
	{
	}

	void TFTimer::UpdateTimer()
	{
		__int64 _nCurrentCount = 0;
		__int64 _nCachedGameCount = m_nElapsedGameCount;
		// Grab the current game count
		QueryPerformanceCounter((LARGE_INTEGER*)&_nCurrentCount);
		// Calculate elapsed game time (current time - start time)
		m_nElapsedGameCount = _nCurrentCount - m_nInitialTimeCount;
		// Calculate elapsed frame time (current time - last time this was called)
		m_nElapsedFrameCount = m_nElapsedGameCount - _nCachedGameCount;
	}

	float TFTimer::GetElapsedTime()
	{
		return static_cast<float>(m_nElapsedFrameCount) / static_cast<float>(m_nCountsPerSecond);
	}

	float TFTimer::GetGameTime()
	{
		return static_cast<float>(m_nElapsedGameCount) / static_cast<float>(m_nCountsPerSecond);
	}

}
