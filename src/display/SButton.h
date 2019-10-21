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
			DWORD raw_color;
			DWORD move_color;

		public:
			MouseEventCallback callback_down, callback_up, callback_move;
			Signal<MouseEvent> callback;

		private:
			void down(MouseEvent e) {
				if (hitTestPoint(e.x, e.y)) {
					if (callback_down) callback_down(e);
					callback(e);
					rect.color = raw_color;
				}
			}

			void up(MouseEvent e) {
				if (hitTestPoint(e.x, e.y)) {
					if(callback_up) callback_up(e);
					callback(e);
					rect.color = move_color;
				}
			}

			void move(MouseEvent e) {
				if (hitTestPoint(e.x, e.y)) {
					if (callback_move) callback_move(e);
					callback(e);
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
				text(0, 0, caption, SFont(txt_size), 0, this),
				raw_color(color), move_color(light_color),
				callback_down(callback_down), callback_up(callback_up), callback_move(callback_move)
			{
				this->x = x;
				this->y = y;
				this->width = width;
				this->height = height;
				text.setheight = height;
				text.setwidth = width;
				text.align = STextImpl::CENTER_ALIGN;
				stage.mouseEventListener.add(this, WM_LBUTTONDOWN, &SButtonImpl::down);
				stage.mouseEventListener.add(this, WM_LBUTTONUP, &SButtonImpl::up);
				stage.mouseEventListener.add(this, WM_MOUSEMOVE, &SButtonImpl::move);
			}

			~SButtonImpl() {
				stage.mouseEventListener.remove(this, WM_LBUTTONDOWN);
				stage.mouseEventListener.remove(this, WM_LBUTTONUP);
				stage.mouseEventListener.remove(this, WM_MOUSEMOVE);
			}

			bool hitTestPoint(int gx, int gy) override { return enabled && rect.hitTestPoint(gx, gy); }
			void paint(HDC hdc) override {
				if (visible) {
					SAlphaHelper sal(this, hdc);
					rect.paint(hdc);
					text.paint(hdc);
				}
			}

			wstringstream& caption() { return text.caption; }
			const wstringstream& caption() const { return text.caption; }
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


		class SliderImpl :public ShapeImpl {
		public:
			using SlideEvent = SimpleEvent<scalar>;
			enum SlideEventEnum { EVENT_SLIDED, EVENT_SLIDING };
			scalar degree;

		private:
			int level, radius;
			DWORD dark_color;
			DWORD light_color;
			SRoundRectImpl selected, background;
			SEllipseImpl circle;
			
			void slide(MouseEvent e) {
				switch (e.mk)
				{
				case WM_LDRAG_MK_BEGIN:
					level = circle.hitTestPoint(e.x, e.y) ? 1 : 0;
					break;
				case WM_LDRAG_MK_END:
				{
					if (level) slideEventListener(SlideEvent(degree, EVENT_SLIDED));
					level = 0;
				}
				break;
				case WM_LDRAG_MK_MOVE:
				{
					if (level) {
						transGlobalPosToLocal(e.x, e.y);
						e.x -= x + radius;
						if (e.x < 0) e.x = 0;
						else if (e.x > width - 2 * radius) e.x = width - 2 * radius;
						circle.x = e.x;
						selected.width = circle.x + radius + selected.radius;
						slideEventListener(SlideEvent(degree = (scalar(e.x) / (width - radius * 2)), EVENT_SLIDING));
					}
				}
				break;
				}
			}

			void move(MouseEvent e) { circle.color = (!level && circle.hitTestPoint(e.x, e.y)) ? light_color : dark_color; }

		public:
			Signal<SlideEvent> slideEventListener;

			SliderImpl(int x, int y, int width, int height, DWORD color, DWORD light_color, ShapeImpl* parent)
				: ShapeImpl(parent), degree(0), level(0), radius(height / 2), dark_color(color), light_color(light_color),
				selected(height / 4, height / 4, 0, height / 4, light_color, PS_SOLID, 3, color, this),
				background(height / 3, height / 3, width - (height * 2 / 3), height / 6, color, PS_NULL, 1, 0, this),
				circle(0, 0, 2 * radius, 2 * radius, true, color, PS_SOLID, 3, color, this)
			{
				this->x = x;
				this->y = y;
				this->width = width;
				this->height = height;
				stage.mouseEventListener.add(this, WM_LDRAG, &SliderImpl::slide);
				stage.mouseEventListener.add(this, WM_MOUSEMOVE, &SliderImpl::move);
			}

			~SliderImpl() {
				stage.mouseEventListener.remove(this, WM_LDRAG);
				stage.mouseEventListener.remove(this, WM_MOUSEMOVE);
			}

			bool hitTestPoint(int gx, int gy) override { 
				return enabled &&
					(selected.hitTestPoint(gx, gy) && background.hitTestPoint(gx, gy) && circle.hitTestPoint(gx, gy));
			}
			void paint(HDC hdc) override {
				if (!visible) return;
				SAlphaHelper sal(this, hdc);
				background.paint(hdc);
				selected.paint(hdc);
				circle.paint(hdc);
			}
		};

		using Slider = sptr<SliderImpl>;
		Slider MakeSlider(int x, int y, int width, int height, DWORD color, DWORD light_color, ShapeImpl* parent = nullptr) {
			return Slider(new SliderImpl(x, y, width, height, color, light_color, parent));
		}

	}
}