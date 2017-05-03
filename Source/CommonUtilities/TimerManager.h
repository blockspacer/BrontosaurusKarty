#pragma once

#include "Timer.h"
#include <vector>
//#include <iostream>


namespace CU
{
	typedef unsigned int TimerHandle;

	class TimerManager
	{
	public:
		TimerManager();
		~TimerManager();

		TimerHandle CreateTimer();
		const Timer& GetTimer(const TimerHandle aHandle) const;
		void UpdateTimers();
		void StartAllTimers();
		void StopAllTimers();
		void StartTimer(const TimerHandle aHandle);
		void StopTimer(const TimerHandle aHandle);
		void ResetTimer(const TimerHandle aHandle);
		void PrintTimers() const;

		size_t Size() const;
	private:
		std::vector<Timer> myTimers;
		std::int64_t myLastTime;
		double myFrequence;
	};
}  