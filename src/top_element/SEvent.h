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
#include "../stdafx.h"
#include <unordered_map>
#include <unordered_set>

#define WM_LDRAG (WM_USER+1)
#define WM_RDRAG (WM_USER+2)
#define WM_FRAME (WM_USER+3)

// Here implemented is a Qt-like signal-slot.

namespace fl {
	namespace events {

		template<typename Para>
		class Slot {
		private:
			class NoneType {};

			class slot_base {
			public:
				virtual void Execute(const Para& para) = 0;
			};

			template<typename Caller>
			class slot_impl :public slot_base {
			private:
				Caller* caller;
				void (Caller::*method)(Para);
			public:
				slot_impl(Caller* caller, void (Caller::*method)(Para)) :caller(caller), method(method) {}
				void Execute(const Para& para) { (caller->*method)(para); }
			};

			template<>
			class slot_impl<NoneType> :public slot_base {
			private:
				void (*method)(Para);
			public:
				slot_impl(void(*method)(Para)) : method(method) {}
				void Execute(const Para& para) { method(para); }
			};

			slot_base* slotBase;

		public:

			void* caller;

			template<typename Caller>
			Slot(Caller* caller, void (Caller::*method)(Para)) :caller((void*)caller) {
				slotBase = new slot_impl<Caller>(caller, method);
			}

			Slot(void(*method)(Para)) : caller(nullptr) {
				slotBase = new slot_impl<NoneType>(method);
			}

			Slot(const Slot<Para>&) = delete;

			Slot(Slot<Para>&& cp) noexcept { 
				slotBase = cp.slotBase;
				cp.slotBase = nullptr;
				caller = cp.caller;
				cp.caller = nullptr;
			}

			~Slot() {
				if (slotBase) delete slotBase;
			}

			void Execute(const Para& para) { slotBase->Execute(para); }
		};

		template<typename Para>
		class Signal {
		private:
			std::unordered_map<DWORD, std::list<Slot<Para>>> slots;

		public:

			~Signal() {}

			template<typename Caller>
			void add(Caller* caller, DWORD type, void(Caller::*method)(Para)) {
				slots[type].push_back(Slot<Para>(caller, method));
			}
			
			template<typename Caller>
			void add(sptr<Caller>& caller, DWORD type, void(Caller::* method)(Para)) {
				slots[type].push_back(Slot<Para>(caller.raw(), method));
			}

			void add(DWORD type, void(*method)(Para)) { slots[type].push_back(Slot<Para>(method)); }
			
			void remove(void* caller, DWORD type) {
				std::list<Slot<Para>>& list = slots[type];
				for (auto it = list.begin(); it != list.end();) {
					if (it->caller == caller) it = list.erase(it);
					else it++;
				}
			}

			void clear() { slots.clear(); }

			void operator()(const Para& para) {
				if (!slots.count(para.type)) return;
				for (auto& it : slots[para.type]) it.Execute(para);
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

#define WM_LDRAG_MK_BEGIN 2
#define WM_LDRAG_MK_END 1
#define WM_LDRAG_MK_MOVE 3
#define WM_RDRAG_MK_BEGIN 2
#define WM_RDRAG_MK_END 1
#define WM_RDRAG_MK_MOVE 3

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