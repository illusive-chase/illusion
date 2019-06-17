#pragma once

#include "SPointer.h"
#include "SEvent.h"

namespace fl {
	namespace display {
		class Shape :public AutoPtr {
		public:
			Shape* parent;
			bool visible, enabled;
			int x, y;
			int width, height;
			Shape(Shape* parent) :x(0), y(0), width(0), height(0), visible(true), enabled(true), parent(parent) {}
			virtual bool hitTestPoint(int gx, int gy) = 0;
			virtual void paint(HDC hdc) = 0;
			virtual ~Shape() {}
			void transLocalPosToGlobal(int& x, int& y);
			void transGlobalPosToLocal(int& x, int& y);
			void hide() { visible = false; }
			void show() { visible = true; }
			virtual void framing() = 0;
		};
	}
}