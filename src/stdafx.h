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

#include "targetver.h"

#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#ifndef ILL_SSE
#define ILL_SSE
#define ILL_SSE_IN_API
#endif

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>


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


using std::string;
using std::wstring;
using std::wstringstream;
typedef unsigned uint;
typedef long long lint;
typedef unsigned long long ulint;
typedef unsigned short ushort;

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
#endif