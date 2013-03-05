#pragma once

#include <Windows.h>

namespace TFCore
{

	class TFTimer
	{
	public:
		TFTimer(void);
		~TFTimer(void);

		void UpdateTimer();
		float GetElapsedTime();
		float GetGameTime();

	private:
		__int64 m_nCountsPerSecond;
		__int64 m_nInitialTimeCount;
		__int64 m_nElapsedGameCount;
		__int64 m_nElapsedFrameCount;
	};

}

