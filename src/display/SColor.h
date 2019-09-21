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
#include "../top_element/Scalar.h"

namespace fl {

#ifdef RGB
#undef RGB
#endif

	// ATTENTION: It is different from the default RGB and is the only CORRECT color format in 3D rendering.
	ILL_INLINE constexpr DWORD RGB3D(BYTE r, BYTE g, BYTE b) { return (r << 16) | (g << 8) | b; }
	ILL_INLINE constexpr DWORD RGB(BYTE r, BYTE g, BYTE b) { return (b << 16) | (g << 8) | r; }
	ILL_INLINE constexpr DWORD RGB(DWORD rgb3d) { return BYTE(rgb3d >> 16) | (rgb3d & 0xff00) | (BYTE(rgb3d) << 16); }

	namespace display {

		// ATTENTION:
		// 1. These colors are NOT compile-time constants because Class Texture will use their address.
		// 2. These colors are mutable, so you can customize them.
		class Color {
		public:
			enum Constant {
				RED = RGB3D(255, 0, 0),
				BLUE = RGB3D(0, 0, 255),
				GREEN = RGB3D(0, 128, 0),
				YELLOW = RGB3D(255, 255, 0),
				PURPLE = RGB3D(128, 0, 128),
				VIOLET = RGB3D(128, 0, 255),
				BLACK = RGB3D(0, 0, 0),
				WHITE = RGB3D(255, 255, 255),
				ORANGE = RGB3D(255, 128, 0),
				SILVER = RGB3D(192, 192, 192),
				GREY = RGB3D(128, 128, 128),
				GOLD = RGB3D(255, 215, 0),
				BROWN = RGB3D(128, 64, 64),
				PINK = RGB3D(255, 128, 192),
				CYAN = RGB3D(0, 255, 255),
				FUCHSIA = RGB3D(255, 0, 255),
				LIME = RGB3D(0, 255, 0)
			};
			virtual ~Color() = 0;
		};
	}
}