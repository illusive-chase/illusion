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

		class BitmapImpl {
		public:
			DWORD* src;
			int width, height;
			BitmapImpl(DWORD* src, int width, int height) :src(src), width(width), height(height) {}
			~BitmapImpl() { if (src) delete[] src; }
		};

		using Bitmap = sptr<BitmapImpl>;
		ILL_INLINE Bitmap MakeBitmap(int width, int height, DWORD* src) {
			return Bitmap(new BitmapImpl(src, width, height));
		}


		// Each SBitmapImpl object keeps a HBITMAP handle, which refers to a bitmap.
		class SBitmapImpl :public ShapeImpl {
		protected:
			HBITMAP hbmp;
			BITMAPINFO info;
			DWORD* keep;

		public:

			SBitmapImpl(int x, int y, Bitmap bmp, ShapeImpl* parent = nullptr)
				:ShapeImpl(parent), keep(nullptr)
			{
				info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				info.bmiHeader.biWidth = bmp->width;
				info.bmiHeader.biHeight = bmp->height;
				info.bmiHeader.biPlanes = 1;
				info.bmiHeader.biBitCount = 32;
				info.bmiHeader.biCompression = BI_RGB;
				hbmp = CreateDIBSection(NULL, &info, DIB_RGB_COLORS, (void**)(&keep), NULL, 0);
				if (bmp->src) {
					DWORD* pdest = keep, * psrc = bmp->src + (bmp->height - 1) * bmp->width;
					for (int i = 0; i < bmp->height; ++i, pdest += bmp->width, psrc -= bmp->width)
						memcpy_s(pdest, bmp->width << 2, psrc, bmp->width << 2);
				}
				this->x = x;
				this->y = y;
				width = bmp->width;
				height = bmp->height;
			}

			DWORD* content() { return keep; }

			// The hit-test area is rectangular.
			virtual bool hitTestPoint(int gx, int gy) override {
				if (!enabled) return false;
				transGlobalPosToLocal(gx, gy);
				return gx >= x && gx <= x + width && gy >= y && gy <= y + height;
			}

			// The function is empty as the bitmap is static.
			virtual void framing() override {}

			~SBitmapImpl() { DeleteObject(hbmp); }

			// It uses TransparentBlt, which allows the bitmap to be displayed transparently.
			virtual void paint(HDC hdc) override {
				if (visible) {
					SAlphaHelper sal(this, hdc);
					int x0 = x, y0 = y;
					transLocalPosToGlobal(x0, y0);
					SetDIBitsToDevice(hdc, x0, y0, width, height, 0, 0, 0, info.bmiHeader.biHeight, keep, &info, DIB_RGB_COLORS);
				}
			}

		};

		using SBitmap = sptr<SBitmapImpl>;
		ILL_INLINE SBitmap MakeSBitmap(int x, int y, Bitmap bmp, ShapeImpl* parent = nullptr) {
			return SBitmap(new SBitmapImpl(x, y, bmp, parent));
		}
	}
}