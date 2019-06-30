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

#include "Shape.h"

namespace fl {
	namespace display {


		// Class SBitmap inherits class AutoPtr indirectly, which means it must be allocated on the heap.
		// Each SBitmap object keeps a HBITMAP handle, which refers to a bitmap.
		// Class SBitmap cannot be inherited.
		class SBitmap :public Shape {
		private:
			HBITMAP hbmp;

		public:
			// ATTENTION: A bitmap created by function CreateBitmap will be selected slower than one created by 
		    // function CreateCompatibleBitmap. So, there seems to be a potential improvement.
			SBitmap(int x, int y, DWORD* src, int width, int height, Shape* parent = nullptr)
				:Shape(parent) {
				hbmp = CreateBitmap(width, height, 1, 32, src);
				this->x = x;
				this->y = y;
				this->width = width;
				this->height = height;
			}

			// It implements the corresponding virtual function of class Shape.
			// The hit-test area is rectangular.
			bool hitTestPoint(int gx, int gy) {
				if (!enabled) return false;
				transGlobalPosToLocal(gx, gy);
				return gx >= x && gx <= x + width && gy >= y && gy <= y + height;
			}

			// It implements the corresponding virtual function of class Shape.
			// The function is empty as the bitmap is static.
			void framing() {}

			~SBitmap() { DeleteObject(hbmp); }

			// It implements the corresponding virtual function of class Shape.
			// It uses TransparentBlt, which allows the bitmap to be displayed transparently.
			void paint(HDC hdc) {
				if (visible) {
					HDC mdc = CreateCompatibleDC(hdc);
					HBITMAP hbp_old = (HBITMAP)SelectObject(mdc, hbmp);
					int x0 = x, y0 = y;
					transLocalPosToGlobal(x0, y0);
					TransparentBlt(hdc, x0, y0, width, height, mdc, 0, 0, width, height, 0x00ff00);
					SelectObject(mdc, hbp_old);
					DeleteDC(mdc);
				}
			}
		};
	}
}