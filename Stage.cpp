#include "Stage.h"

fl::display::Stage stage;
HWND g_hWnd = NULL;

DWORD fl::display::Stage::message(const wstring& test, const wstring& TITLE, UINT mode_MB, bool fixed) {
	if (fixed) return MessageBox(g_hWnd, test.c_str(), TITLE.c_str(), mode_MB);
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
		for (Shape* shape : children) shape->paint(hdc);
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

bool fl::display::Stage::showConsole() {
	if (console_show) return false;
	return console_show = true, ShowWindow(GetConsoleWindow(), SW_SHOW);
}

bool fl::display::Stage::hideConsole() {
	if (!console_show) return false;
	return console_show = false, ShowWindow(GetConsoleWindow(), SW_HIDE);
}

void fl::display::Stage::destroy() {
	removeEventListener();
}

void fl::display::Stage::framing() { 
	frameEventListener(fl::events::FrameEvent(WM_FRAME));
	for (Shape* child : children) child->framing();
}

RECT fl::display::Stage::getWindowArea() const {
	RECT r;
	GetWindowRect(g_hWnd, &r);
	return r;
}

RECT fl::display::Stage::getStageArea() const {
	RECT r;
	GetClientRect(g_hWnd, &r);
	return r;
}

void fl::display::Stage::setMouse(int x, int y) {
	mouseX = x;
	mouseY = y;
	SetCursorPos(x, y);
}
