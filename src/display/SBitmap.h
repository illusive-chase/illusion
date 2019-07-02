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


		// Each SBitmapImpl object keeps a HBITMAP handle, which refers to a bitmap.
		class SBitmapImpl final :public ShapeImpl {
		private:
			HBITMAP hbmp;

		public:
			// ATTENTION: A bitmap created by function CreateBitmap will be selected slower than one created by 
		    // function CreateCompatibleBitmap. So, there seems to be a potential improvement.
			SBitmapImpl(int x, int y, DWORD* src, int width, int height, Shape parent = nullptr)
				:ShapeImpl(parent) {
				hbmp = CreateBitmap(width, height, 1, 32, src);
				this->x = x;
				this->y = y;
				this->width = width;
				this->height = height;
			}

			// The hit-test area is rectangular.
			bool hitTestPoint(int gx, int gy) override {
				if (!enabled) return false;
				transGlobalPosToLocal(gx, gy);
				return gx >= x && gx <= x + width && gy >= y && gy <= y + height;
			}

			// The function is empty as the bitmap is static.
			void framing() override {}

			~SBitmapImpl() { DeleteObject(hbmp); }

			// It uses TransparentBlt, which allows the bitmap to be displayed transparently.
			void paint(HDC hdc) override {
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

		using SBitmap = sptr<SBitmapImpl>;
		ILL_INLINE SBitmap MakeSBitmap(int x, int y, DWORD* src, int width, int height, Shape parent = nullptr) {
			return SBitmap(new SBitmapImpl(x, y, src, width, height, parent));
		}
	}
}