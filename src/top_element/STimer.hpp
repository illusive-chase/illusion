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

namespace fl {
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

			ILL_INLINE bool tick(DWORD dt) {
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

std::list<fl::time::Timer*> fl::time::Timer::global_timers;

DWORD fl::time::Timer::base_time = 0;
DWORD fl::time::Timer::delay_time = 0;

void fl::time::Timer::global_tick(DWORD dt) {
	delay_time = dt;
	for (auto it = global_timers.begin(); it != global_timers.end();) {
		if ((*it)->tick(dt)) ++it;
		else {
			delete *it;
			it = global_timers.erase(it);
		}
	}
}

void fl::time::Timer::destroy() {
	for (Timer* it : global_timers) {
		delete it;
	}
}

fl::time::Timer::Timer(DWORD interval, int count) :interval(interval), curr_time(0), count(count) {
	global_timers.push_back(this);
}

fl::time::Timer::~Timer() {
	timerEventListener.clear();
}