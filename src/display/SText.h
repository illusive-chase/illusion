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
			SFont font;
			wstringstream caption; // mutable

			// It is used to listen for painting events.
			// In fact, the event is only responded when function STextImpl::paint is called.
			fl::events::Signal<fl::events::SimpleEvent<STextImpl*>> paintEventListener;
			
			STextImpl(int x, int y, const wstring& caption, const SFont& font = SFont(20), Shape parent = Shape(nullptr))
				:ShapeImpl(parent), caption(caption), font(font)
			{
				this->x = x;
				this->y = y;
				width = 0;
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
				paintEventListener(this); // respond
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

		ILL_INLINE SText MakeSText(int x, int y, const wstring& caption, const SFont& font = SFont(20), Shape parent = Shape(nullptr)) {
			return SText(new STextImpl(x, y, caption, font, parent));
		}
	}
}