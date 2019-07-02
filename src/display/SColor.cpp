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
#include "SColor.h"
using namespace fl::display;
using namespace fl;

DWORD Color::RED = RGB3D(255, 0, 0);
DWORD Color::BLUE = RGB3D(0, 0, 255);
DWORD Color::GREEN = RGB3D(0, 128, 0);
DWORD Color::YELLOW = RGB3D(255, 255, 0);
DWORD Color::PURPLE = RGB3D(128, 0, 128);
DWORD Color::VIOLET = RGB3D(128, 0, 255);
DWORD Color::BLACK = RGB3D(0, 0, 0);
DWORD Color::WHITE = RGB3D(255, 255, 255);
DWORD Color::ORANGE = RGB3D(255, 128, 0);
DWORD Color::SILVER = RGB3D(192, 192, 192);
DWORD Color::GREY = RGB3D(128, 128, 128);
DWORD Color::GOLD = RGB3D(255, 215, 0);
DWORD Color::BROWN = RGB3D(128, 64, 64);
DWORD Color::PINK = RGB3D(255, 128, 192);
DWORD Color::CYAN = RGB3D(0, 255, 255);
DWORD Color::FUCHSIA = RGB3D(255, 0, 255);
DWORD Color::LIME = RGB3D(0, 255, 0);

Color::~Color() {}