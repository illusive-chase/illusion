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
#include "SText.h"
#include "SGeomShape.h"

namespace fl {
	namespace display {

		class SButtonImpl :public ShapeImpl {
		private:
			SRectImpl rect;
			STextImpl text;
			bool first_paint;
			DWORD raw_color;
			DWORD move_color;

		public:
			const wstring caption;
			MouseEventCallback callback_down, callback_up, callback_move;

		private:
			void down(MouseEvent e) {
				if (hitTestPoint(e.x, e.y)) {
					if (callback_down) callback_down(e);
					rect.color = raw_color;
				}
			}

			void up(MouseEvent e) {
				if (hitTestPoint(e.x, e.y)) {
					if(callback_up) callback_up(e);
					rect.color = move_color;
				}
			}

			void move(MouseEvent e) {
				if (hitTestPoint(e.x, e.y)) {
					if (callback_move) callback_move(e);
					rect.color = move_color;
				} else rect.color = raw_color;
			}

		public:
			SButtonImpl(int x, int y, int width, int height, DWORD color, DWORD light_color,
						const wstring& caption, int txt_size,
						MouseEventCallback callback_down = nullptr,
						MouseEventCallback callback_up = nullptr,
						MouseEventCallback callback_move = nullptr,
						ShapeImpl* parent = nullptr)
				:ShapeImpl(parent), rect(0, 0, width, height, true, color, PS_NULL, 0, 0, this),
				text(0, 0, caption, SFont(txt_size), this), caption(caption), 
				first_paint(true), raw_color(color), move_color(light_color),
				callback_down(callback_down), callback_up(callback_up), callback_move(callback_move)
			{
				this->x = x;
				this->y = y;
				this->width = width;
				this->height = height;
				stage.mouseEventListener.add(this, WM_LBUTTONDOWN, &SButtonImpl::down);
				stage.mouseEventListener.add(this, WM_LBUTTONUP, &SButtonImpl::up);
				stage.mouseEventListener.add(this, WM_MOUSEMOVE, &SButtonImpl::move);
			}


			bool hitTestPoint(int gx, int gy) override { return rect.hitTestPoint(gx, gy); }
			void paint(HDC hdc) override {
				if (first_paint) {
					SIZE sz = { 0,0 };
					SGDIObject keep(hdc, CreateFont(text.font.size, 0, 0, 0, text.font.weight, 0, 0, 0,
													GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, 
													DEFAULT_QUALITY, FF_DONTCARE, TRANSLATE_FONT[text.font.style].c_str()));
					GetTextExtentPoint(hdc, text.caption.str().c_str(), (int)text.caption.str().length(), &sz);
					text.x = (width - sz.cx) / 2;
					text.y = (height - sz.cy) / 2;
					first_paint = false;
				}
				rect.paint(hdc);
				text.paint(hdc);
			}
			void framing() override {}

		};

		using SButton = sptr<SButtonImpl>;
		SButton MakeSButton(int x, int y, int width, int height, DWORD color, DWORD light_color,
							const wstring& caption, int txt_size,
							MouseEventCallback callback_down = nullptr,
							MouseEventCallback callback_up = nullptr,
							MouseEventCallback callback_move = nullptr,
							ShapeImpl* parent = nullptr) {
			return SButton(new SButtonImpl(
				x, y, width, height, color, light_color, caption, txt_size, callback_down, callback_up, callback_move, parent));
		}

	}
}