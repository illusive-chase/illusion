#pragma once
#include "Shape.h"

namespace fl {
	namespace display {
		class SEllipse :public Shape {
		public:
			bool filled;
			COLORREF color;
			DWORD ps;
			int pwid;
			COLORREF pcolor;
			void framing() {}
			virtual ~SEllipse() {}
			SEllipse(int _x, int _y, int _width, int _height, bool _filled, COLORREF _color, int pen_style_PS = PS_SOLID, int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), Shape* parent = nullptr) :Shape(parent) {
				x = _x;
				y = _y;
				width = _width;
				height = _height;
				filled = _filled;
				color = _color;
				ps = pen_style_PS;
				pwid = pen_width;
				pcolor = pen_color;
			}
			bool hitTestPoint(int gx, int gy) {
				if (!filled) return false;
				transGlobalPosToLocal(gx, gy);
				scalar a = (gx - x) / scalar(width) - scalar(0.5), b = (gy - y) / scalar(height) - scalar(0.5);
				return (a * a + b * b) < scalar(1);
			}
		private:
			void paint(HDC hdc) {
				if (visible) {
					int x0 = x, y0 = y;
					transLocalPosToGlobal(x0, y0);
					DeleteObject(SelectObject(hdc, CreatePen(ps, pwid, pcolor)));
					if (filled) {
						DeleteObject(SelectObject(hdc, CreateSolidBrush(color)));
						Ellipse(hdc, x0, y0, x0 + width, y0 + height);
					} else Ellipse(hdc, x0, y0, x0 + width, y0 + height);
				}
			}
		};

		class SLine :public Shape {
		public:
			DWORD ps;
			int pwid;
			COLORREF pcolor;
			int x0, y0, x1, y1;
			void framing() {}
			virtual ~SLine() {}
			SLine(int x0, int y0, int x1, int y1, int pen_style_PS = PS_INSIDEFRAME, int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), Shape* parent = nullptr) :Shape(parent), x0(x0), y0(y0), x1(x1), y1(y1) {
				x = min(x0, x1);
				y = min(y0, y1);
				this->x1 -= x, this->x0 -= x;
				this->y1 -= y, this->y0 -= y;
				width = abs(x1 - x0);
				height = abs(y1 - y0);
				ps = pen_style_PS;
				pwid = pen_width;
				pcolor = pen_color;
				enabled = false;
			}
			SLine(int x0, int y0, scalar rad, int length, int PEN_STYLE = PS_SOLID, int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), Shape* parent = nullptr) :Shape(parent) {
				int x1 = x0 + int(round(length * cos(rad)));
				int y1 = y0 + int(round(length * sin(rad)));
				SLine(x0, y0, x1, y1, PEN_STYLE, pen_width, pen_color, parent);
			}
			bool hitTestPoint(int gx, int gy) { return false; }
		private:
			void paint(HDC hdc) {
				if (visible) {
					DeleteObject(SelectObject(hdc, CreatePen(ps, pwid, pcolor)));
					int gx = x, gy = y;
					transLocalPosToGlobal(gx, gy);
					MoveToEx(hdc, x0 + gx, y0 + gy, NULL);
					LineTo(hdc, x1 + gx, y1 + gy);
				}
			}
		};

		class SRect :public Shape {
		public:
			bool filled;
			COLORREF color;
			DWORD ps;
			int pwid;
			COLORREF pcolor;
			void framing() {}
			virtual ~SRect() {}
			SRect(int _x, int _y, int _width, int _height, bool _filled, COLORREF _color, int pen_style_PS = PS_INSIDEFRAME, int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), Shape* parent = nullptr) :Shape(parent) {
				x = _x;
				y = _y;
				width = _width;
				height = _height;
				filled = _filled;
				color = _color;
				ps = pen_style_PS;
				pwid = pen_width;
				pcolor = pen_color;
			}
			bool hitTestPoint(int gx, int gy) {
				if (!filled) return false;
				transGlobalPosToLocal(gx, gy);
				return gx >= x && gx <= x + width && gy >= y && gy <= y + height;
			}
		private:
			void paint(HDC hdc) {
				if (visible) {
					int x0 = x, y0 = y;
					transLocalPosToGlobal(x0, y0);
					DeleteObject(SelectObject(hdc, CreatePen(ps, pwid, pcolor)));
					if (filled) {
						DeleteObject(SelectObject(hdc, CreateSolidBrush(color)));
						Rectangle(hdc, x0, y0, x0 + width, y0 + height);
					} else Rectangle(hdc, x0, y0, x0 + width, y0 + height);
				}
			}
		};
	}
}