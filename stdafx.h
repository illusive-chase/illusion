/// @file stdafx.h
/// @brief 标准系统包含文件的包含文件，或是经常使用但不常更改的项目特定的包含文件\n
/// 预使用命名空间和定义宏
/// @date 2019/3/31

#pragma once

#include "targetver.h"

#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#define _CRT_SECURE_NO_WARNINGS

#ifndef ILL_SSE
#define ILL_SSE
#define ILL_SSE_IN_API
#endif

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// 在此处引用程序需要的其他标头
#pragma comment(lib,"msimg32.lib")
#include <cstdio>
#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <map>
#include <ctime>
#include <exception>
#include <algorithm>


using std::vector;
using std::list;
using std::map;
using std::string;
using std::wstring;
using std::wstringstream;
typedef unsigned uint;
typedef long long lint;
typedef unsigned long long ulint;
typedef unsigned short ushort;

#define WM_FRAME (WM_USER+1)
#define WM_RELEASE (WM_USER+2)
#undef max
#undef min
using std::max;
using std::min;