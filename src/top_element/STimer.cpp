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