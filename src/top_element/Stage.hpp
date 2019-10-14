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

namespace fl {
	namespace display {

		// Class Stage is actually a singleton but is not implemented as such(but this should be improved).
		// It should never be inherited.
		class Stage :public SpriteImpl {
		public:
			fl::events::Signal<fl::events::KeyboardEvent> keyboardEventListener;
			fl::events::Signal<fl::events::MouseEvent> mouseEventListener;
			fl::events::Signal<fl::events::SystemEvent> systemEventListener;
			fl::events::Signal<fl::events::FrameEvent> frameEventListener;


			bool console_show;
			union {
				struct { int mouseX, mouseY; };
				POINT mouseP;
			};

			Stage() :SpriteImpl(0, 0, nullptr), mouseX(0), mouseY(0), console_show(false) {}
			~Stage() {}
			bool addConsole(const wstring& TITLE = L"Console");
			bool freeConsole();
			bool showConsole();
			bool hideConsole();
			bool showFrameDelay(bool value, bool read_only = false);

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

fl::display::Stage stage;

DWORD fl::display::Stage::message(const wstring& test, const wstring& TITLE, UINT mode_MB, bool fixed) {
	if (fixed) return MessageBox(fl::System::g_hWnd, test.c_str(), TITLE.c_str(), mode_MB);
	MessageBox(NULL, test.c_str(), TITLE.c_str(), mode_MB);
	return -1;
}

void fl::display::Stage::removeEventListener() {
	keyboardEventListener.clear();
	mouseEventListener.clear();
	systemEventListener.clear();
	frameEventListener.clear();
}

void fl::display::Stage::paint(HDC hdc) {
	Rectangle(hdc, -1, -1, width + 1, height + 1);
	if (visible) {
		for (Shape shape : children) shape->paint(hdc);
	}
}


bool fl::display::Stage::addConsole(const wstring& TITLE) {
	if (!AllocConsole()) return false;
	if (!freopen("CONIN$", "r+t", stdin)) return false;
	if (!freopen("CONOUT$", "w+t", stdout)) return false;
	console_show = true;
	if (!SetConsoleTitle(TITLE.c_str())) return false;
	return true;
}

bool fl::display::Stage::freeConsole() {
	return FreeConsole();
}

bool fl::display::Stage::showConsole() {
	if (console_show) return false;
	return console_show = true, ShowWindow(GetConsoleWindow(), SW_RESTORE);
}

bool fl::display::Stage::hideConsole() {
	if (!console_show) return false;
	return console_show = false, ShowWindow(GetConsoleWindow(), SW_HIDE) && ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
}

bool fl::display::Stage::showFrameDelay(bool value, bool read_only) {
	static bool show_frame = true;
	if (read_only) return show_frame;
	if (show_frame != value) return show_frame = value, true;
	return false;
}

void fl::display::Stage::destroy() {
	removeEventListener();
}

void fl::display::Stage::framing() {
	frameEventListener(fl::events::FrameEvent(WM_FRAME));
	for (Shape& child : children) child->framing();
}

RECT fl::display::Stage::getWindowArea() const {
	RECT r;
	GetWindowRect(fl::System::g_hWnd, &r);
	return r;
}

RECT fl::display::Stage::getStageArea() const {
	RECT r;
	GetClientRect(fl::System::g_hWnd, &r);
	return r;
}

void fl::display::Stage::setMouse(int x, int y) {
	mouseX = x;
	mouseY = y;
	ClientToScreen(System::g_hWnd, &mouseP);
	SetCursorPos(mouseX, mouseY);
	mouseX = x;
	mouseY = y;
}
