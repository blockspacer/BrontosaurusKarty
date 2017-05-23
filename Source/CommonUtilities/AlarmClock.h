#pragma once
#include "CUTime.h"
#include "StopWatch.h"

namespace CU
{
	class CAlarmClock
	{
	public:
		CAlarmClock();
		CAlarmClock(const Time aTimeTillRinging, const std::function<void(void)>& aCallback);
		~CAlarmClock();

		void Init(const Time aTimeTillRinging, const std::function<void(void)>& aCallback);
		void Update();

	private:
		std::function<void(void)> myCallback;
		CStopWatch myStopWatch;
		Time myDoneTime;
	};
}
