#pragma once


#include "Shape.h"
#include "SFont.h"
#include "SEvent.h"

namespace fl {
	namespace display {
		class SText :public Shape {
		private:
			HFONT hfont;
			SFont font;

		public:
			wstringstream caption;
			fl::events::Signal<fl::events::SimpleEvent<SText*>> paintEventListener;
			
			SText(int _x, int _y, const wstring& _caption, const SFont& _font = SFont(20), Shape* parent = nullptr) 
				:Shape(parent), caption(_caption), font(_font)
			{
				x = _x;
				y = _y;
				width = 0;
				height = 0;
				hfont = CreateFont(font.size, 0, 0, 0,
					font.weight, font.italic, font.underline, font.midline,
					GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
					TRANSLATE_FONT[font.style].c_str());
				enabled = false;
			}
			void framing() {}
			~SText() { DeleteObject(hfont); }
			bool hitTestPoint(int gx, int gy) { return false; }
			virtual void paint(HDC hdc) {
				paintEventListener(this);
				if (visible) {
					SetBkMode(hdc, TRANSPARENT);
					SetTextColor(hdc, font.color);
					HFONT hfont_old = (HFONT)SelectObject(hdc, hfont);
					SIZE sz = { 0,0 };
					GetTextExtentPoint(hdc, caption.str().c_str(), (int)caption.str().length(), &sz);
					width = sz.cx;
					height = sz.cy;
					int x0 = x, y0 = y;
					transLocalPosToGlobal(x0, y0);
					TextOut(hdc, x0, y0, caption.str().c_str(), (int)caption.str().length());
					SelectObject(hdc, hfont_old);
				}
			}
		};
	}
}