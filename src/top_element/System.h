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

#define WM_FRAME (WM_USER+1)

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow);

namespace fl {

	class System {
	private:
		friend int APIENTRY ::wWinMain(_In_ HINSTANCE hInstance,
			_In_opt_ HINSTANCE hPrevInstance,
			_In_ LPWSTR    lpCmdLine,
			_In_ int       nCmdShow);
		static constexpr int MAX_LOADSTRING = 100;
		static HINSTANCE g_hInstance;
		static HDC g_hdc;
		static HDC g_hmemdc;
		static HBITMAP g_hbitmap;
		static HWND g_hWnd;
		static int init_nCmdShow;
		static HINSTANCE hInst;                                // 当前实例
		static WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
		static WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

		static ATOM                MyRegisterClass(HINSTANCE hInstance);
		static BOOL                InitInstance(HINSTANCE, int, int, int, int, int);
		static INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		System() = delete;
		~System() = delete;

	public:
		static bool InitWindow(const wstring& title = L"Program", int xpos = 20, int ypos = 20, int width = 1024, int height = 768);
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