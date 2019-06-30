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
			static std::list<Timer*> global_timers;
			static void global_tick(int dt);
			static DWORD base_time;
			static DWORD delay_time;

			Signal<TimerEvent> timerEventListener;

			Timer(int interval, int count = -1);
			~Timer();

		private:

			const int interval;
			int curr_time;
			int count;

			ILL_INLINE bool tick(int dt){ 
				curr_time += dt;
				if (curr_time >= interval) timerEventListener(TimerEvent()), --count, curr_time -= interval;
				return count;
			}

			Timer(const Timer& cp) :interval(0) {}
			void operator =(const Timer& cp){}
		};
	}
}