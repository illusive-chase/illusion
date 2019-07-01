#include "STimer.h"

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