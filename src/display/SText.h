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
#include "SFont.h"

namespace fl {
	namespace display {

		class STextImpl;

		using SText = sptr<STextImpl>;

		// Each STextImpl object keeps a HFONT handle, which refers to a font.
		class STextImpl :public ShapeImpl {
		private:
			HFONT hfont;
			
		public:
			int setwidth;
			int setheight;
			SFont font;
			wstringstream caption;
			bool wordbreak;
			bool autobreak;
			enum AlignEnum {
				LEFT_ALIGN = DT_LEFT,
				CENTER_ALIGN = DT_CENTER,
				RIGHT_ALIGN = DT_RIGHT
			} align;

			// It is used to listen for painting events.
			// In fact, the event is only responded when function STextImpl::paint is called.
			fl::events::Signal<fl::events::SimpleEvent<STextImpl&>> paintEventListener;
			
			STextImpl(int x, int y, const wstring& caption, const SFont& font = SFont(20), int setwidth = 0, ShapeImpl * parent = nullptr)
				:ShapeImpl(parent), setwidth(setwidth), setheight(0), caption(caption), font(font), wordbreak(true), autobreak(false), align(LEFT_ALIGN)
			{
				this->x = x;
				this->y = y;
				width = setwidth;
				height = 0;
				hfont = CreateFont(font.size, 0, 0, 0,
					font.weight, font.italic, font.underline, font.midline,
					GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
					TRANSLATE_FONT[font.style].c_str());
				enabled = false;
			}

			// The function is empty as the text is static.
			// But it does not mean the text is not mutable, only means the text will not change by itself.
			virtual void framing() override {}

			virtual ~STextImpl() { DeleteObject(hfont); }

			// The text should never be hit.
			// If necessary, use hidden rectangle for hit-test instead.
			bool hitTestPoint(int gx, int gy) override { return false; }

			// It updates the width and height, so the size number is correct even if you change the caption.
			void paint(HDC hdc) override {
				paintEventListener(*this); // respond
				if (visible) {
					SAlphaHelper sal(this, hdc);
					int x0 = x, y0 = y;
					transLocalPosToGlobal(x0, y0);
					unsigned flag = align;
					if (autobreak) {
						flag |= DT_EDITCONTROL;
						if (wordbreak) flag |= DT_WORDBREAK;
					}
					if (setheight > 0) flag |= DT_VCENTER | DT_SINGLELINE;
					SetTextColor(hdc, font.color);
					HFONT hfont_old = (HFONT)SelectObject(hdc, hfont);
					RECT r = { x0, y0,x0 + width,y0 + height };
					DrawTextW(hdc, caption.str().c_str(), -1, &r, DT_CALCRECT | flag);
					if (setwidth <= 0) width = r.right - r.left;
					else r.right = r.left + setwidth;
					if (setheight <= 0) height = r.bottom - r.top;
					else r.bottom = r.top + setheight;
					DrawTextW(hdc, caption.str().c_str(), -1, &r, flag);
					width = r.right - r.left;
					height = r.bottom - r.top;
					SelectObject(hdc, hfont_old);
				}
			}
		};

		ILL_INLINE SText MakeSText(int x, int y, const wstring& caption, const SFont& font = SFont(20), int setwidth = 0, ShapeImpl * parent = nullptr) {
			return SText(new STextImpl(x, y, caption, font, setwidth, parent));
		}
	}
}