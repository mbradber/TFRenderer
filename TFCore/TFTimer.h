#pragma once

#include <Windows.h>

namespace TFCore
{

	class TFTimer
	{
	public:
		TFTimer();
		~TFTimer();

		void UpdateTimer();
		float GetElapsedTime() const;
		float GetGameTime()    const;

	private:
		__int64 m_nCountsPerSecond;
		__int64 m_nInitialTimeCount;
		__int64 m_nElapsedGameCount;
		__int64 m_nElapsedFrameCount;
	};

}

