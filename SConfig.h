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
#include "stdafx.h"


namespace fl {
	constexpr int MILISECOND_PER_FRAME = 30;                                   // miliseconds per frame
	constexpr clock_t CLOCKS = CLOCKS_PER_SEC / 1000;                          // clocks per milisecond
	const wstring TRANSLATE_FONT[] = { L"Î¢ÈíÑÅºÚ",L"ËÎÌå",L"ºÚÌå",L"·ÂËÎ" };    // basic fonts
	constexpr int MAX_LOADSTRING = 100;                                        // a maximum length related with the windows form

#ifdef ILL_USE_DOUBLE_PRECISION
	constexpr double PI = 3.141592653589793238462643;
	constexpr double EPS = DBL_EPSILON;
	constexpr double INF = DBL_MAX;
#else
	constexpr float PI = 3.141592653589793238462643f;
	constexpr float EPS = FLT_EPSILON;
	constexpr float INF = FLT_MAX;
#endif
}