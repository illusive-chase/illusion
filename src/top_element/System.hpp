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
#include "..\stdafx.h"
#include "..\resource.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
using std::max;
using std::min;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					  _In_opt_ HINSTANCE hPrevInstance,
					  _In_ LPWSTR    lpCmdLine,
					  _In_ int       nCmdShow);

namespace fl {

	class System {
	public:
		class WindowStyle {
		public:
			bool has_caption;
			bool is_tool;
			bool has_menu;
			bool has_maxbox;
			bool has_minbox;
			bool can_resize;
			BYTE alpha;
			WindowStyle(bool has_caption = true, bool is_tool = false, bool has_menu = true,
						bool has_maxbox = true, bool has_minbox = true, bool can_resize = true,
						BYTE alpha = 0xff)
				:has_caption(has_caption), is_tool(is_tool), has_menu(has_menu),
				has_maxbox(has_maxbox), has_minbox(has_minbox), can_resize(can_resize),
				alpha(alpha) {}

			operator long() {
				long ret = 0L;
				if (has_caption) {
					ret |= WS_CAPTION;
					if (has_menu) {
						ret |= WS_SYSMENU;
						if (has_maxbox) ret |= WS_MAXIMIZEBOX;
						if (has_minbox) ret |= WS_MINIMIZEBOX;
					}
				} else ret |= WS_POPUP;
				if (can_resize) ret |= WS_THICKFRAME;
				return ret;
			}

		};

	private:
		friend int APIENTRY::wWinMain(_In_ HINSTANCE hInstance,
									  _In_opt_ HINSTANCE hPrevInstance,
									  _In_ LPWSTR    lpCmdLine,
									  _In_ int       nCmdShow);
		static constexpr int MAX_LOADSTRING = 100;
		static HINSTANCE g_hInstance;
		static HDC g_hdc;
		static HDC g_hmemdc;
		static HBITMAP g_hbitmap;
		
		static int init_nCmdShow;
		static HINSTANCE hInst;                                // 当前实例
		static WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
		static WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

		static ATOM                MyRegisterClass(HINSTANCE hInstance);
		static BOOL                InitInstance(HINSTANCE, int, int, int, int, int, WindowStyle);
		static INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		System() = delete;
		~System() = delete;

	public:

		static HWND g_hWnd;

		static bool InitWindow(const wstring& title = L"Program", int xpos = 20, int ypos = 20, int width = 1024, int height = 768, WindowStyle ws = WindowStyle());
		static bool HideWindow();
		static bool ShowWindow();

		static void Setup();
		static wstring CurrentExe();

#ifdef GetCurrentDirectory
#undef GetCurrentDirectory
#endif
		static wstring GetCurrentDirectory();

#ifdef SetCurrentDirectory
#undef SetCurrentDirectory
#endif
		static bool SetCurrentDirectory(const wstring& dir);

#ifdef GetEnvironmentVariable
#undef GetEnvironmentVariable
#endif
		static wstring GetEnvironmentVariable(const wstring& name);
		static void PrintLastError();

		static bool IsAdmin(bool& isadmin);

	};
}


#endif


#include <ctime>
#include <algorithm>
#include "SConfig.h"
#include "Stage.h"
#include "STimer.h"
#include <clocale>
#include <ShlObj_core.h>

using fl::System;

extern fl::display::Stage stage;
HINSTANCE System::g_hInstance;
HDC System::g_hdc;
HDC System::g_hmemdc;
HBITMAP System::g_hbitmap;
HWND System::g_hWnd;
int System::init_nCmdShow;
HINSTANCE System::hInst;                                // 当前实例
WCHAR System::szTitle[System::MAX_LOADSTRING];                  // 标题栏文本
WCHAR System::szWindowClass[System::MAX_LOADSTRING];            // 主窗口类名




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	System::g_hInstance = hInstance;
	System::init_nCmdShow = nCmdShow;

	// 初始化全局字符串
	//LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//LoadStringW(hInstance, IDC_ILLUSION, System::szWindowClass, System::MAX_LOADSTRING);
	wcscpy_s(System::szWindowClass, L"ILLUSION");
	System::MyRegisterClass(hInstance);

	std::setlocale(LC_ALL, "");
	

	System::Setup();
	// 执行应用程序初始化:
	
	//HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ILLUSION));

	MSG msg = {};
	DWORD curr_time = 0, prev_time = 0;

	// 主消息循环:
	fl::time::Timer::base_time = GetTickCount();

	LARGE_INTEGER li = {};
	li.QuadPart = -100000;
	HANDLE m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);

	SetFocus(System::g_hWnd);

	if (m_hTimer) {
		SetWaitableTimer(m_hTimer, &li, 1, NULL, NULL, FALSE);

		while (1) {
			if (!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) && WaitForSingleObject(m_hTimer, INFINITE) == WAIT_OBJECT_0) {
				curr_time = GetTickCount();
				fl::time::Timer::global_tick(DWORD(curr_time - fl::time::Timer::base_time));
				fl::time::Timer::base_time = curr_time;
				if (curr_time - prev_time > fl::MILISECOND_PER_FRAME) {
					InvalidateRect(System::g_hWnd, NULL, TRUE);
					stage.framing();
					prev_time = curr_time;
				}
			} else {
				if (WM_QUIT == msg.message) break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		CloseHandle(m_hTimer);
	}
	stage.destroy();
	return (int)msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM fl::System::MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	//wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ILLUSION));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ILLUSION);
	wcex.lpszClassName = szWindowClass;
	//wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL fl::System::InitInstance(HINSTANCE hInstance, int nCmdShow, int xpos, int ypos, int width, int height, System::WindowStyle ws) {
	hInst = hInstance; // 将实例句柄存储在全局变量中
	DWORD ex_ws = 0;
	if (ws.is_tool) ex_ws |= WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
	if (~ws.alpha) ex_ws |= WS_EX_LAYERED;
	g_hWnd = CreateWindowExW(ex_ws, szWindowClass, szTitle, ws,
		xpos, ypos, width, height, nullptr, nullptr, hInstance, nullptr);
	if (!g_hWnd) return FALSE;
	if (~ws.alpha) SetLayeredWindowAttributes(g_hWnd, 0, ws.alpha, LWA_ALPHA);
	::ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK fl::System::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	using namespace fl::events;
	static int pre_leftdown = 0;
	static int pre_rightdown = 0;
	static int leftdown = 0;
	static int rightdown = 0;
	switch (message) {
	case WM_ERASEBKGND:
	{
		return 1;
	}
	break;
	case WM_CREATE:
	{
		stage.systemEventListener(SystemEvent(message));
	}
	break;
	case WM_SIZE:
	{
		stage.width = LOWORD(lParam);
		stage.height = HIWORD(lParam);
		stage.systemEventListener(SystemEvent(message));
	}
	break;
	case WM_KEYDOWN:
	case WM_CHAR:
	case WM_KEYUP:
	{
		stage.keyboardEventListener(KeyboardEvent(message, (int)wParam));
	}
	break;
	case WM_LBUTTONDOWN:leftdown = 1; goto WM_MOUSE;
	case WM_LBUTTONUP:leftdown = 0; goto WM_MOUSE;
	case WM_RBUTTONDOWN:rightdown = 1; goto WM_MOUSE;
	case WM_RBUTTONUP:rightdown = 0; goto WM_MOUSE;
	WM_MOUSE:
	{
		stage.mouseEventListener(MouseEvent(message, stage.mouseX, stage.mouseY, (int)wParam));
		if (leftdown != pre_leftdown)
			stage.mouseEventListener(MouseEvent(WM_LDRAG, stage.mouseX, stage.mouseY, (leftdown << 1 | pre_leftdown)));
		if (rightdown != pre_rightdown)
			stage.mouseEventListener(MouseEvent(WM_RDRAG, stage.mouseX, stage.mouseY, (rightdown << 1 | pre_rightdown)));
		pre_leftdown = leftdown;
		pre_rightdown = rightdown;
	}
	break;
	case WM_MOUSEMOVE:
	{
		GetCursorPos(&stage.mouseP);
		ScreenToClient(g_hWnd, &stage.mouseP);
		pre_leftdown = leftdown;
		pre_rightdown = rightdown;
	}
	break;
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
	{
		GetCursorPos(&stage.mouseP);
		ScreenToClient(g_hWnd, &stage.mouseP);
		stage.mouseEventListener(MouseEvent(message, stage.mouseX, stage.mouseY, (int)wParam));
	}
	break;
#if 0
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		stage.systemEventListener(SystemEvent(message));
		// 分析菜单选择:
		switch (wmId) {
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
#endif
	case WM_PAINT:
	{
		if (leftdown && pre_leftdown)
			stage.mouseEventListener(MouseEvent(WM_LDRAG, stage.mouseX, stage.mouseY, 3));
		if (rightdown && pre_rightdown)
			stage.mouseEventListener(MouseEvent(WM_RDRAG, stage.mouseX, stage.mouseY, 3));
		stage.mouseEventListener(MouseEvent(WM_MOUSEMOVE, stage.mouseX, stage.mouseY, (int)wParam));

		stage.systemEventListener(SystemEvent(message));
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HDC memhdc = CreateCompatibleDC(hdc);
		HBITMAP hbmp = CreateCompatibleBitmap(hdc, stage.width, stage.height);
		HBITMAP hbmp_old = (HBITMAP)SelectObject(memhdc, hbmp);
		SetBkMode(hdc, TRANSPARENT);
		stage.paint(memhdc);

		static DWORD prev_time = 0;
		static DWORD curr_time = 0;
		static int cnt = 0;
		if (++cnt > 7) {
			prev_time = curr_time ? curr_time : fl::time::Timer::base_time;
			curr_time = fl::time::Timer::base_time;
			cnt = 0;
		}
		if (stage.showFrameDelay(false, true)) {
			WCHAR itow[20];
			_itow_s(int((curr_time - prev_time) >> 3), itow, 20, 10);
			TextOut(memhdc, 50, 50, itow, lstrlenW(itow));
		}
		TransparentBlt(hdc, 0, 0, stage.width, stage.height, memhdc, 0, 0, stage.width, stage.height, 0x00ff00);
		SelectObject(memhdc, hbmp_old);
		DeleteObject(hbmp);
		DeleteDC(memhdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		stage.systemEventListener(SystemEvent(message));
		PostQuitMessage(0);
	}
	break;
	case WM_FRAME:
	{
		stage.frameEventListener(FrameEvent(message));
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// “关于”框的消息处理程序。
INT_PTR CALLBACK fl::System::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


bool fl::System::InitWindow(const wstring& title, int xpos, int ypos, int width, int height, System::WindowStyle ws) {
	wcscpy_s(szTitle, title.c_str());
	//if (!InitInstance(g_hInstance, init_nCmdShow, xpos, ypos, width + 17, height + 60, ws)) return false;
	if (!InitInstance(g_hInstance, init_nCmdShow, xpos, ypos, width, height + 40, ws)) return false;
	//stage.width = width + 17, stage.height = height + 17;
	stage.width = width, stage.height = height;
	return true;
}

ILL_INLINE bool fl::System::HideWindow() {
	return ::ShowWindow(g_hWnd, SW_HIDE) && ::ShowWindow(g_hWnd, SW_MINIMIZE);
}

ILL_INLINE bool fl::System::ShowWindow() {
	return ::ShowWindow(g_hWnd, SW_RESTORE);
}

wstring fl::System::CurrentExe() {
	WCHAR ptr[MAX_PATH];
	if (!GetModuleFileNameW(g_hInstance, ptr, MAX_PATH)) return L"";
	return wstring(ptr);
}

wstring fl::System::GetCurrentDirectory() {
	WCHAR* ptr = nullptr;
	DWORD sz = GetCurrentDirectoryW(0, ptr);
	if (!sz) return L"";
	ptr = new WCHAR[sz];
	GetCurrentDirectoryW(sz, ptr);
	wstring ret(ptr);
	delete[] ptr;
	return ret;
}

bool fl::System::SetCurrentDirectory(const wstring & dir) {
	return SetCurrentDirectoryW(dir.c_str());
}

wstring fl::System::GetEnvironmentVariable(const wstring & name) {
	WCHAR* ptr = nullptr;
	DWORD sz = GetEnvironmentVariableW(name.c_str(), ptr, 0);
	if (!sz) return L"";
	ptr = new WCHAR[sz];
	GetEnvironmentVariableW(name.c_str(), ptr, sz);
	wstring ret(ptr);
	delete[] ptr;
	return ret;
}

void fl::System::PrintLastError() {
	DWORD le = GetLastError();
	HLOCAL hlocal = NULL;
	DWORD sys_locale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
	BOOL f = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, le, sys_locale, (LPWSTR)&hlocal, 0, NULL);
	if (!f) {
		HMODULE hDll = LoadLibraryExW(L"netmsg.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);
		if (hDll != NULL) {
			f = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
				hDll, le, sys_locale, (LPWSTR)&hlocal, 0, NULL);
			FreeLibrary(hDll);
		}
	}
	if (f && (hlocal != NULL))
	{
		std::wcout << (PCWSTR)LocalLock(hlocal);
		LocalFree(hlocal);
	} else std::wcout << L"Fail to get last error.\n";
}

bool fl::System::IsAdmin(bool& isadmin) {
	static bool bSet = false;
	static BOOL bRet = FALSE;
	if (bSet) {
		isadmin = bRet;
		return true;
	}
	{
		HANDLE hToken = NULL;
		TOKEN_ELEVATION_TYPE* type = nullptr;
		DWORD dwSize;
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) return bool(bSet = false);
		if (GetTokenInformation(hToken, TokenElevationType, type, sizeof(TOKEN_ELEVATION_TYPE), &dwSize) && type) {
			BYTE adminSID[SECURITY_MAX_SID_SIZE];
			dwSize = sizeof(adminSID);
			CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSID, &dwSize);
			if (*type == TokenElevationTypeLimited) {
				HANDLE unfiltered_token = NULL;
				GetTokenInformation(hToken, TokenLinkedToken, (VOID*)&unfiltered_token, sizeof(HANDLE), &dwSize);
				if (CheckTokenMembership(unfiltered_token, &adminSID, &bRet)) bSet = true;
				CloseHandle(unfiltered_token);
			} else {
				bRet = IsUserAnAdmin();
				bSet = true;
			}
		}
		CloseHandle(hToken);
		return bSet;
	}
}