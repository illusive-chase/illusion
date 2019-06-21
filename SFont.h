#pragma once

#include "Scalar.h"

namespace fl {
	namespace display {
		
		

		class SFont {
		public:
			const int size, weight;
			const DWORD italic, underline, midline;
			const COLORREF color;
			const enum STYLE {
				WRYH,
				SONG,
				HEI,
				FANGSONG
			} style;
			SFont(int _size, STYLE _style = WRYH, bool _italic = false, bool _bold = false,
				bool _underline = false, bool _midline = false, COLORREF _color = RGB(0, 0, 0)) :
				size(_size), style(_style), italic(_italic), weight(_bold ? 700 : 400), underline(_underline),
				midline(_midline), color(_color) {
			}
		};
	}
}