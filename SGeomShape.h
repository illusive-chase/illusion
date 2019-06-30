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

		// Class SEllipse inherits class AutoPtr indirectly, which means it must be allocated on the heap.
		// Class SEllipse cannot be inherited.
		class SEllipse :public Shape {
		public:
			bool filled;
			COLORREF color; // fill color
			DWORD ps; // pen's style
			int pwid; // pen's thickness
			COLORREF pcolor; // pen's color

			// It implements the corresponding virtual function of class Shape and is empty as the shape is static.
			void framing() {}

			~SEllipse() {}

			SEllipse(int x, int y, int width, int height, bool filled, COLORREF color,
				int pen_style_PS = PS_SOLID, int pen_width = 1,
				COLORREF pen_color = RGB(0, 0, 0), Shape* parent = nullptr) 
				:Shape(parent) 
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

			// It implements the corresponding virtual function of class Shape.
			// The hit-test area is an accurately calculated ellipse.
			bool hitTestPoint(int gx, int gy) {
				if (!filled || !enabled) return false;
				transGlobalPosToLocal(gx, gy);
				scalar a = (gx - x) / scalar(width) - scalar(0.5), b = (gy - y) / scalar(height) - scalar(0.5);
				return (a * a + b * b) < scalar(1);
			}

		private:
			// It implements the corresponding virtual function of class Shape.
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


		// Class SLine inherits class AutoPtr indirectly, which means it must be allocated on the heap.
		// Class SLine cannot be inherited.
		class SLine :public Shape {
		public:
			DWORD ps; // pen's style
			int pwid; // pen's thickness
			COLORREF pcolor; //pen's color
			int x0, y0, x1, y1; // This line points from (x0, y0) to (x1, y1)

			// It implements the corresponding virtual function of class Shape and is empty as the shape is static.
			void framing() {} 

			~SLine() {}

			SLine(int x0, int y0, int x1, int y1, int pen_style_PS = PS_SOLID,
				int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), Shape* parent = nullptr) 
				:Shape(parent), x0(x0), y0(y0), x1(x1), y1(y1) 
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
			SLine(int x0, int y0, scalar rad, int length, int PEN_STYLE = PS_SOLID,
				int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), Shape* parent = nullptr) 
				:Shape(parent)
			{
				int x1 = x0 + int(round(length * cos(rad)));
				int y1 = y0 + int(round(length * sin(rad)));
				SLine(x0, y0, x1, y1, PEN_STYLE, pen_width, pen_color, parent);
			}

			// Lines will never be hit.
			bool hitTestPoint(int gx, int gy) { return false; } 

		private:
			// It implements the corresponding virtual function of class Shape.
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

		// Class SRect inherits class AutoPtr indirectly, which means it must be allocated on the heap.
		// Class SRect cannot be inherited.
		class SRect :public Shape {
		public:
			bool filled;
			COLORREF color; // fill color
			DWORD ps; // pen's style
			int pwid; // pen's thickness
			COLORREF pcolor; // pen's color

			// It implements the corresponding virtual function of class Shape and is empty as the shape is static.
			void framing() {}

			~SRect() {}
			
			SRect(int x, int y, int width, int height, bool filled, COLORREF color, 
				int pen_style_PS = PS_INSIDEFRAME, int pen_width = 1, COLORREF pen_color = RGB(0, 0, 0), 
				Shape* parent = nullptr) :Shape(parent) 
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

			// It implements the corresponding virtual function of class Shape.
			bool hitTestPoint(int gx, int gy) {
				if (!filled || !enabled) return false;
				transGlobalPosToLocal(gx, gy);
				return gx >= x && gx <= x + width && gy >= y && gy <= y + height;
			}

		private:
			// It implements the corresponding virtual function of class Shape.
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