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
				COLORREF pen_color = RGB(0, 0, 0), ShapeImpl* parent = nullptr) 
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
				int a2 = gx - x - width / 2, b2 = gy - y - height / 2;
				a2 *= a2, b2 *= b2;
				int h2 = height * height, w2 = width * width;
				return (a2 * h2 + b2 * w2) <= h2 * w2;
			}

			void paint(HDC hdc) override {
				if (visible) {
					SAlphaHelper sal(this, hdc);
					int x0 = x, y0 = y;
					transLocalPosToGlobal(x0, y0);
					SGDIObject keep(hdc, CreatePen(ps, pwid, pcolor));
					if (filled) {
						SGDIObject keep(hdc, CreateSolidBrush(color));
						Ellipse(hdc, x0, y0, x0 + width, y0 + height);
					} else Ellipse(hdc, x0, y0, x0 + width, y0 + height);
				}
			}
		};

		using SEllipse = sptr<SEllipseImpl>;
		ILL_INLINE SEllipse MakeSEllipse(int x, int y, int width, int height, bool filled, COLORREF color,
			int pen_style_PS = PS_SOLID, int pen_width = 1,
			COLORREF pen_color = RGB(0, 0, 0), ShapeImpl* parent = nullptr) {
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
				int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), ShapeImpl* parent = nullptr) 
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
				int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), ShapeImpl* parent = nullptr) 
				:ShapeImpl(parent), x0(x0), y0(y0)
			{
				x1 = x0 + int(round(length * cos(rad)));
				y1 = y0 + int(round(length * sin(rad)));
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

			// Lines will never be hit.
			bool hitTestPoint(int gx, int gy) override { return false; }

			void paint(HDC hdc) override {
				if (visible) {
					SAlphaHelper sal(this, hdc);
					SGDIObject keep(hdc, CreatePen(ps, pwid, pcolor));
					int gx = x, gy = y;
					transLocalPosToGlobal(gx, gy);
					MoveToEx(hdc, x0 + gx, y0 + gy, NULL);
					LineTo(hdc, x1 + gx, y1 + gy);
				}
			}
		};

		using SLine = sptr<SLineImpl>;
		ILL_INLINE SLine MakeSLine(int x0, int y0, int x1, int y1, int pen_style_PS = PS_SOLID,
			int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), ShapeImpl* parent = nullptr) {
			return SLine(new SLineImpl(x0, y0, x1, y1, pen_style_PS, pen_width, pen_color, parent));
		}

		ILL_INLINE SLine MakeSLine(int x0, int y0, scalar rad, int length, int pen_style_PS = PS_SOLID,
			int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), ShapeImpl* parent = nullptr) {
			return SLine(new SLineImpl(x0, y0, rad, length, pen_style_PS, pen_width, pen_color, parent));
		}

		class SRectImpl :public ShapeImpl {
		public:
			bool filled;
			COLORREF color; // fill color
			DWORD ps; // pen's style
			int pwid; // pen's thickness
			COLORREF pcolor; // pen's color

			virtual void framing() override {}

			virtual ~SRectImpl() {}
			
			SRectImpl(int x, int y, int width, int height, bool filled, COLORREF color, 
				int pen_style_PS = PS_INSIDEFRAME, int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), 
				ShapeImpl* parent = nullptr) :ShapeImpl(parent) 
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

			virtual bool hitTestPoint(int gx, int gy) override {
				if (!filled || !enabled) return false;
				transGlobalPosToLocal(gx, gy);
				return gx >= x && gx <= x + (int)width && gy >= y && gy <= y + (int)height;
			}

			virtual void paint(HDC hdc) override {
				if (visible) {
					SAlphaHelper sal(this, hdc);
					int x0 = x, y0 = y;
					transLocalPosToGlobal(x0, y0);
					SGDIObject keep(hdc, CreatePen(ps, pwid, pcolor));
					if (filled) {
						SGDIObject keep(hdc, CreateSolidBrush(color));
						Rectangle(hdc, x0, y0, x0 + width, y0 + height);
					} else Rectangle(hdc, x0, y0, x0 + width, y0 + height);
				}
			}
		};

		using SRect = sptr<SRectImpl>;
		ILL_INLINE SRect MakeSRect(int x0, int y0, int width, int height, bool filled, COLORREF color, int pen_style_PS = PS_SOLID,
			int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), ShapeImpl* parent = nullptr) {
			return SRect(new SRectImpl(x0, y0, width, height, filled, color, pen_style_PS, pen_width, pen_color, parent));
		}


		class SRoundRectImpl : public ShapeImpl {
		public:
			COLORREF color; // fill color
			DWORD ps; // pen's style
			int pwid; // pen's thickness
			COLORREF pcolor; // pen's color
			int radius; // height/2

			SRoundRectImpl(int x, int y, int width, int radius, COLORREF color,
					   int pen_style_PS = PS_INSIDEFRAME, int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0),
					   ShapeImpl * parent = nullptr) :ShapeImpl(parent), radius(radius)
			{
				this->x = x;
				this->y = y;
				this->height = radius * 2;
				this->width = max(height, width);
				this->color = color;
				ps = pen_style_PS;
				pwid = pen_width;
				pcolor = pen_color;
			}

			virtual void framing() override {}
			virtual bool hitTestPoint(int gx, int gy) override {
				if (!enabled) return false;
				transGlobalPosToLocal(gx, gy);
				gx -= x, gy -= y;
				return (gx >= radius && gx <= (width - radius) && gy >= 0 && gy <= height)
					|| ((gx - radius) * (gx - radius) + (gy - radius) * (gy - radius)) < radius * radius
					|| ((gx - width + radius) * (gx - width + radius) + (gy - radius) * (gy - radius)) < radius * radius;
			}

			virtual void paint(HDC hdc) override {
				if (visible) {
					SAlphaHelper sal(this, hdc);
					int x0 = x, y0 = y;
					transLocalPosToGlobal(x0, y0);
					SGDIObject keep(hdc, CreatePen(ps, pwid, pcolor));
					SGDIObject keep2(hdc, CreateSolidBrush(color));
					Ellipse(hdc, x0, y0, x0 + height, y0 + height);
					Ellipse(hdc, x0 + width - height, y0, x0 + width, y0 + height);
					{
						SGDIObject keep(hdc, CreatePen(PS_NULL, 1, 0));
						Rectangle(hdc, x0 + radius, y0, x0 + width - radius, y0 + height);
					}
					MoveToEx(hdc, x0 + radius, y0, NULL);
					LineTo(hdc, x0 + width - radius, y0);
					MoveToEx(hdc, x0 + radius, y0 + height, NULL);
					LineTo(hdc, x0 + width - radius, y0 + height);
				}
			}

		};

		using SRoundRect = sptr<SRoundRectImpl>;
		ILL_INLINE SRoundRect MakeSRoundRect(int x0, int y0, int width, int radius, COLORREF color, int pen_style_PS = PS_SOLID,
								   int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), ShapeImpl * parent = nullptr) {
			return SRoundRect(new SRoundRectImpl(x0, y0, width, radius, color, pen_style_PS, pen_width, pen_color, parent));
		}

	}
}