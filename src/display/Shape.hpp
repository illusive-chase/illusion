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

#include "../top_element/SPointer.h"
#include "../top_element/SEvent.h"

namespace fl {
	namespace display {

		class ShapeImpl;

		using Shape = sptr<ShapeImpl>;

		// graphics base class
		class ShapeImpl {
		protected:

			class SGDIObject {
			public:
				HDC hdc;
				HGDIOBJ obj;
				HGDIOBJ obj_old;
				SGDIObject(HDC hdc, HGDIOBJ obj) :hdc(hdc), obj(obj), obj_old(SelectObject(hdc, obj)) {}
				~SGDIObject() { SelectObject(hdc, obj_old); DeleteObject(obj); }
			};

		public:
			const ShapeImpl* parent; // the container(see its interface in SpriteImpl.h) containing the instance of this class
			bool visible, enabled; // visible: whether to be drawn; enabled: whether to response hit-test
			int x, y; // screen coordinates
			int width, height; // width and height of enclosing rectangle
			ShapeImpl(ShapeImpl* parent) :x(0), y(0), width(0), height(0), visible(true), enabled(true), parent(parent) {}

			// It returns true if and only if the shape hit the screen coordinates (gx, gy).
			// But when 'enabled' is false, it always returns false.
			virtual bool hitTestPoint(int gx, int gy) = 0;

			// It is called in the same name function of 'parent' to perform drawing.
			// In fact, what always happens is that it is called directly or indirectly
			// in the same name function of class Stage.
			virtual void paint(HDC hdc) = 0;

			virtual ~ShapeImpl() {}

			void transLocalPosToGlobal(int& x, int& y) const;
			void transGlobalPosToLocal(int& x, int& y) const;
			void hide() { visible = false; }
			void show() { visible = true; }

			// It is called in the same name function of 'parent' in every LOGICAL FRAME.
			// In fact, what always happens is that it is called directly or indirectly in the same name function of class Stage.
			// For static graphics, this function is always empty; 
			// for other graphics, this function is responsible for some frame animations.
			// You can also override this method in your custom derived class.
			// ATTENTION: The term LOGICAL FRAME is completely different from the term PAINT FRAME.
			// See the same name function in Stage.h.
			virtual void framing() = 0;
		};


		
	}
}

void fl::display::ShapeImpl::transLocalPosToGlobal(int& x, int& y) const {
	for (const ShapeImpl* fa = parent; fa; fa = fa->parent) {
		x += fa->x;
		y += fa->y;
	}
}

void fl::display::ShapeImpl::transGlobalPosToLocal(int& x, int& y) const {
	for (const ShapeImpl* fa = parent; fa; fa = fa->parent) {
		x -= fa->x;
		y -= fa->y;
	}
}