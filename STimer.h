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

			inline bool tick(int dt){ 
				curr_time += dt;
				if (curr_time >= interval) timerEventListener(TimerEvent()), --count, curr_time -= interval;
				return count;
			}

			Timer(const Timer& cp) :interval(0) {}
			void operator =(const Timer& cp){}
		};
	}
}