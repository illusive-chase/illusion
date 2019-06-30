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
#include "Scalar.h"

namespace fl {

	// ATTENTION: It is different from the default macro RGB and is the only CORRECT color format in 3D rendering.
	ILL_INLINE DWORD RGB3D(BYTE r, BYTE g, BYTE b) { return (r << 16) | (g << 8) | b; }

	namespace display {

		// ATTENTION:
		// 1. These colors are NOT compile-time constants because Class Texture will use their address.
		// 2. These colors are mutable, so you can customize them.
		class Color {
		public:
			static DWORD RED, BLUE, GREEN, YELLOW, PURPLE, VIOLET, BLACK, WHITE, ORANGE, SILVER, GREY, GOLD, BROWN, PINK, CYAN, FUCHSIA, LIME;
			virtual ~Color() = 0;
		};

	}
}