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

#include "../display/Sprite.h"
#include "STimer.h"
using namespace fl::events;
using namespace fl::time;

namespace fl {
	namespace display {

		// Class Stage is actually a singleton but is not implemented as such(but this should be improved).
		// It should never be inherited.
		class Stage :public SpriteImpl {
		public:
			Signal<KeyboardEvent> keyboardEventListener;
			Signal<MouseEvent> mouseEventListener;
			Signal<SystemEvent> systemEventListener;
			Signal<FrameEvent> frameEventListener;


			bool console_show;
			int mouseX, mouseY;

			Stage() :SpriteImpl(0, 0, nullptr), mouseX(0), mouseY(0), console_show(false) {}
			~Stage() {}
			bool addConsole(const wstring& TITLE = L"Console");
			bool showConsole();
			bool hideConsole();

			void destroy();
			void framing();

			RECT getWindowArea() const;
			RECT getStageArea() const;
			void setMouse(int x, int y);

			// It returns macro ID... like macro IDOK
			DWORD message(const wstring& test, const wstring& TITLE, UINT mode_MB, bool fixed);
			
			void removeEventListener();
			ILL_INLINE bool hitTestPoint(int x, int y) { return true; }
			void paint(HDC hdc);
		};


	}
}