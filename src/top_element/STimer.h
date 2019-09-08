/*
MIT License

Copyright (c) 2019 illusive-chase

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*/
#pragma once
#include "SPointer.h"
#include "SEvent.h"

namespace fl{
	namespace time {

		using namespace events;

		class Timer {
		public:
			static void global_tick(DWORD dt);
			static void destroy();
			static DWORD base_time;
			static DWORD delay_time;

			Signal<TimerEvent> timerEventListener;

			Timer(DWORD interval, int count = -1);
			~Timer();

		private:
			static std::list<Timer*> global_timers;
			const DWORD interval;
			DWORD curr_time;
			int count;

			ILL_INLINE bool tick(DWORD dt){
				DWORD pres_time = curr_time + dt;
				if (pres_time < curr_time) return count;
				if (pres_time >= interval) {
					timerEventListener(TimerEvent());
					--count;
					curr_time = pres_time - interval;
				}
				return count;
			}

			Timer(const Timer&) = delete;
			Timer(Timer&&) = delete;
			void operator =(const Timer&) = delete;
			void operator =(Timer&&) = delete;
		};
	}
}