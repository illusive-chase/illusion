#pragma once
#include "stdafx.h"

namespace fl {
	namespace events {

		class NoneType {
		public:
			static NoneType * ptr;
		};

		template<typename Para>
		class Slot {
		private:
			class slot_base {
			public:
				virtual void Execute(Para& para) = 0;
			};

			template<typename Caller>
			class slot_impl :public slot_base {
			private:
				Caller* caller;
				void (Caller::*method)(Para);
			public:
				slot_impl(Caller* caller, void (Caller::*method)(Para)) :caller(caller), method(method) {}
				void Execute(Para& para) { (caller->*method)(para); }
			};

			template<>
			class slot_impl<NoneType> :public slot_base {
			private:
				NoneType* caller;
				void (*method)(Para);
			public:
				slot_impl(NoneType*, void (*method)(Para)) :caller(nullptr), method(method) {}
				void Execute(Para& para) { method(para); }
			};

			slot_base* slotBase;

		public:

			void const *caller;
			DWORD type;

			template<typename Caller>
			Slot(Caller* caller, DWORD type, void (Caller::*method)(Para)) :caller((void*)caller), type(type) {
				slotBase = new slot_impl<Caller>(caller, method);
			}

			Slot(NoneType*, DWORD type, void(*method)(Para)) : caller(nullptr), type(type) {
				slotBase = new slot_impl<NoneType>(nullptr, method);
			}

			Slot(const Slot<Para>& cp) { 
				slotBase = cp.slotBase;
				caller = cp.caller;
				type = cp.type;
			}

			void Execute(Para& para) { slotBase->Execute(para); }

			void release() { delete slotBase; }
		};

		template<typename Para>
		class Signal {
		private:
			std::list<Slot<Para>> slots;
		public:

			~Signal() { clear(); }

			template<typename Caller>
			void add(Caller* caller, DWORD type, void(Caller::*method)(Para)) { slots.push_back(Slot<Para>(caller, type, method)); }
			
			void add(DWORD type, void(*method)(Para)) { slots.push_back(Slot<Para>(NoneType::ptr, type, method)); }
			
			void remove(void* caller) {
				for (auto it = slots.begin(); it != slots.end();) {
					if (it->caller == caller) {
						delete it;
						it = slots.erase(it);
					} else ++it;
				}
			}
			void clear() { 
				for (auto it = slots.begin(); it != slots.end(); ++it) it->release();
				slots.clear();
			}
			void operator()(Para para) { 
				for (auto it = slots.begin(); it != slots.end(); ++it) {
					if (para.type == it->type) it->Execute(para);
				}
			}
		};

		template<typename T>
		class SimpleEvent {
		public:
			T value;
			DWORD type;
			SimpleEvent(T value) :value(value), type(0) {}
			SimpleEvent(T value, DWORD type) :value(value), type(type) {}
		};

		class SystemEvent {
		public:
			DWORD type;
			SystemEvent(DWORD type) :type(type) {}
		};

		class KeyboardEvent {
		public:
			int keyCode;
			DWORD type;
			KeyboardEvent(DWORD type, int keyCode) :type(type), keyCode(keyCode) {}
		};

		class MouseEvent {
		public:
			int x, y, mk;
			DWORD type;

			//MOUSE_WHEEL: only use x as zDelta*120, y as fwKeys
			MouseEvent(DWORD type, int x, int y, int mk) :type(type), x(x), y(y), mk(mk) {}
		};

		class TimerEvent {
		public:
			DWORD type;
			TimerEvent() :type(WM_TIMER) {}
		};

		class FrameEvent {
		public:
			DWORD type;
			FrameEvent(DWORD type) :type(type) {}
		};

		typedef void(*KeyboardEventCallback) (KeyboardEvent e);
		typedef void(*MouseEventCallback) (MouseEvent e);
		typedef void(*TimerEventCallback) (TimerEvent e);
		typedef void(*SystemEventCallback) (SystemEvent e);
		typedef void(*FrameEventCallback) (FrameEvent e);
	}
}