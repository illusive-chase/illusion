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

// This file encapsulates some basic geometry that is drawn using GDI.

namespace fl {
	namespace display {

		class SEllipseImpl :public ShapeImpl {
		public:
			bool filled;
			COLORREF color; // fill color
			DWORD ps; // pen's style
			int pwid; // pen's thickness
			COLORREF pcolor; // pen's color

			void framing() override {}

			virtual ~SEllipseImpl() {}

			SEllipseImpl(int x, int y, int width, int height, bool filled, COLORREF color,
				int pen_style_PS = PS_SOLID, int pen_width = 1,
				COLORREF pen_color = RGB(0, 0, 0), Shape parent = nullptr) 
				:ShapeImpl(parent) 
			{
				this->x = x;
				this->y = y;
				this->width = width;
				this->height = height;
				this->filled = filled;
				this->color = color;
				ps = pen_style_PS;
				pwid = pen_width;
				pcolor = pen_color;
			}

			// The hit-test area is an accurately calculated ellipse.
			bool hitTestPoint(int gx, int gy) override {
				if (!filled || !enabled) return false;
				transGlobalPosToLocal(gx, gy);
				scalar a = (gx - x) / scalar(width) - scalar(0.5), b = (gy - y) / scalar(height) - scalar(0.5);
				return (a * a + b * b) < scalar(1);
			}

			void paint(HDC hdc) override {
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

		using SEllipse = sptr<SEllipseImpl>;
		ILL_INLINE SEllipse MakeSEllipse(int x, int y, int width, int height, bool filled, COLORREF color,
			int pen_style_PS = PS_SOLID, int pen_width = 1,
			COLORREF pen_color = RGB(0, 0, 0), Shape parent = nullptr) {
			return SEllipse(new SEllipseImpl(x, y, width, height, filled, color, pen_style_PS, pen_width, pen_color, parent));
		}


		class SLineImpl :public ShapeImpl {
		public:
			DWORD ps; // pen's style
			int pwid; // pen's thickness
			COLORREF pcolor; //pen's color
			int x0, y0, x1, y1; // This line points from (x0, y0) to (x1, y1)

			void framing() override {}

			virtual ~SLineImpl() {}

			SLineImpl(int x0, int y0, int x1, int y1, int pen_style_PS = PS_SOLID,
				int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), Shape parent = nullptr) 
				:ShapeImpl(parent), x0(x0), y0(y0), x1(x1), y1(y1) 
			{
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

			// Another constructor uses polar coordinates.
			SLineImpl(int x0, int y0, scalar rad, int length, int pen_style_PS = PS_SOLID,
				int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), Shape parent = nullptr) 
				:ShapeImpl(parent)
			{
				int x1 = x0 + int(round(length * cos(rad)));
				int y1 = y0 + int(round(length * sin(rad)));
				SLineImpl(x0, y0, x1, y1, pen_style_PS, pen_width, pen_color, parent);
			}

			// Lines will never be hit.
			bool hitTestPoint(int gx, int gy) override { return false; }

			void paint(HDC hdc) override {
				if (visible) {
					DeleteObject(SelectObject(hdc, CreatePen(ps, pwid, pcolor)));
					int gx = x, gy = y;
					transLocalPosToGlobal(gx, gy);
					MoveToEx(hdc, x0 + gx, y0 + gy, NULL);
					LineTo(hdc, x1 + gx, y1 + gy);
				}
			}
		};

		using SLine = sptr<SLineImpl>;
		ILL_INLINE SLine MakeSLine(int x0, int y0, int x1, int y1, int pen_style_PS = PS_SOLID,
			int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), Shape parent = nullptr) {
			return SLine(new SLineImpl(x0, y0, x1, y1, pen_style_PS, pen_width, pen_color, parent));
		}

		ILL_INLINE SLine MakeSLine(int x0, int y0, scalar rad, int length, int pen_style_PS = PS_SOLID,
			int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), Shape parent = nullptr) {
			return SLine(new SLineImpl(x0, y0, rad, length, pen_style_PS, pen_width, pen_color, parent));
		}

		class SRectImpl :public ShapeImpl {
		public:
			bool filled;
			COLORREF color; // fill color
			DWORD ps; // pen's style
			int pwid; // pen's thickness
			COLORREF pcolor; // pen's color

			// It implements the corresponding virtual function of class ShapeImpl and is empty as the shape is static.
			void framing() override {}

			virtual ~SRectImpl() {}
			
			SRectImpl(int x, int y, int width, int height, bool filled, COLORREF color, 
				int pen_style_PS = PS_INSIDEFRAME, int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), 
				Shape parent = nullptr) :ShapeImpl(parent) 
			{
				this->x = x;
				this->y = y;
				this->width = width;
				this->height = height;
				this->filled = filled;
				this->color = color;
				ps = pen_style_PS;
				pwid = pen_width;
				pcolor = pen_color;
			}

			bool hitTestPoint(int gx, int gy) override {
				if (!filled || !enabled) return false;
				transGlobalPosToLocal(gx, gy);
				return gx >= x && gx <= x + width && gy >= y && gy <= y + height;
			}

			void paint(HDC hdc) override {
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

		using SRect = sptr<SRectImpl>;
		ILL_INLINE SRect MakeSRect(int x0, int y0, int width, int height, bool filled, COLORREF color, int pen_style_PS = PS_SOLID,
			int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), Shape parent = nullptr) {
			return SRect(new SRectImpl(x0, y0, width, height, filled, color, pen_style_PS, pen_width, pen_color, parent));
		}
	}
}