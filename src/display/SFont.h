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
	namespace display {
		
		// Class SFont contains information about the font.
		class SFont {
		public:
			const int size, weight; // The weight equals 700 for bold fonts and 400 for normal fonts.
			const DWORD italic, underline, midline; // The type is DWORD to match the parameter type of the function CreateFont.
			const COLORREF color;
			
			// These are the corresponding indexes of some Chinese fonts.
			// See fl::TRANSLATE_FONT in SConfig.h
			const enum STYLE {
				WRYH,
				SONG,
				HEI,
				FANGSONG
			} style;

			SFont(int size, STYLE style = WRYH, bool italic = false, bool bold = false,
				bool underline = false, bool midline = false, COLORREF color = RGB(0, 0, 0)) :
				size(size), style(style), italic(italic), weight(bold ? 700 : 400), underline(underline),
				midline(midline), color(color) {
			}
		};
	}
}