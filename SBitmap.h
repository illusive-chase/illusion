#pragma once

#include "Shape.h"

namespace fl {
	namespace display {
		class SBitmap :public Shape {
		public:
			HBITMAP hbmp;
			SBitmap(int _x, int _y, DWORD* src, int w, int h, Shape* parent = nullptr)
				:Shape(parent) {
				hbmp = CreateBitmap(w, h, 1, 32, src);
				x = _x;
				y = _y;
				width = w;
				height = h;
			}
			bool hitTestPoint(int gx, int gy) {
				transGlobalPosToLocal(gx, gy);
				return gx >= x && gx <= x + width && gy >= y && gy <= y + height;
			}
			void framing() {}
			virtual ~SBitmap() {}
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