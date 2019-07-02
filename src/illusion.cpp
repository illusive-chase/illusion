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
#include "stdafx.h"
#include "illusion.h"
#include <ctime>
#include <algorithm>
#include "top_element/Scalar.h"
#include "top_element/Stage.h"
#include "top_element/STimer.h"

using namespace fl;
using namespace fl::display;
using namespace fl::time;


// 全局变量和静态变量:
HINSTANCE g_hInstance;
HDC g_hdc;
HDC g_hmemdc;
HBITMAP g_hbitmap;
extern HWND g_hWnd;
extern Stage stage;



int init_nCmdShow;
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, int, int, int, int);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_hInstance = hInstance;
	init_nCmdShow = nCmdShow;

	// 初始化全局字符串
	//LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_ILLUSION, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	Setup();
	// 执行应用程序初始化:

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ILLUSION));

	MSG msg;
	tagRECT lprect;
	DWORD curr_time = 0, prev_time = 0;

	// 主消息循环:
	Timer::base_time = GetTickCount();

	LARGE_INTEGER li = {};
	li.QuadPart = -100000;
	HANDLE m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	SetWaitableTimer(m_hTimer, &li, 1, NULL, NULL, FALSE);

	while (WaitForSingleObject(m_hTimer, INFINITE) == WAIT_OBJECT_0) {
		if (!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			curr_time = GetTickCount();
			Timer::global_tick(DWORD(curr_time - Timer::base_time));
			Timer::base_time = curr_time;
			if (curr_time - prev_time > MILISECOND_PER_FRAME) {
				if (GetClientRect(g_hWnd, &lprect)) {
					InvalidateRect(g_hWnd, &lprect, TRUE);
					UpdateWindow(g_hWnd);
				}
				stage.framing();
				prev_time = curr_time;
			} else Sleep(0);
		} else {
			if (WM_QUIT == msg.message) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	CloseHandle(m_hTimer);
	stage.destroy();
	return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ILLUSION));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ILLUSION);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, int xpos, int ypos, int width, int height) {
	hInst = hInstance; // 将实例句柄存储在全局变量中
	g_hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		xpos, ypos, width, height, nullptr, nullptr, hInstance, nullptr);
	if (!g_hWnd) {
		return FALSE;
	}

	ShowWindow(g_hWnd, nCmdShow);
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
	case WM_ERASEBKGND:
	{
		return 1;
	}
	break;
	case WM_CREATE:
	{
		//SetTimer(hWnd, 1234, CLOCKS_PER_SEC / FRAME_PER_SEC, NULL);
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
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
	{
		POINT ms;
		GetCursorPos(&ms);
		stage.mouseX = ms.x;
		stage.mouseY = ms.y;
		stage.mouseEventListener(MouseEvent(message, ms.x, ms.y, (int)wParam));
	}
	break;
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
	case WM_PAINT:
	{
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
			prev_time = curr_time ? curr_time : Timer::base_time;
			curr_time = Timer::base_time;
			cnt = 0;
		}
		WCHAR itow[20];
		_itow_s(int((curr_time - prev_time) >> 3), itow, 20, 10);
		TextOut(memhdc, 50, 50, itow, lstrlenW(itow));

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
	case WM_RELEASE:
	{
		delete reinterpret_cast<AutoPtr*>(wParam);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
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

bool fl::InitWindow(const wstring& title, int xpos, int ypos, int width, int height) {
	wcscpy(szTitle, title.c_str());
	if (!InitInstance(g_hInstance, init_nCmdShow, xpos, ypos, width + 17, height + 60)) {
		return FALSE;
	}
	stage.width = width + 17, stage.height = height + 17;
	return TRUE;
}