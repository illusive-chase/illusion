#pragma once
#include "Scalar.h"

namespace fl {

	constexpr DWORD RGB3D(BYTE r, BYTE g, BYTE b) { return (r << 16) | (g << 8) | b; }

	namespace display {
		class Color {
		public:
			static DWORD RED, BLUE, GREEN, YELLOW, PURPLE, VIOLET, BLACK, WHITE, ORANGE, SILVER, GREY, GOLD, BROWN, PINK, CYAN, FUCHSIA, LIME;
			virtual ~Color() = 0;
		};

	}
}