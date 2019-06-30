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
#include "SGeomMath.h"

// I have referenced the implementation method used by intel.
// See https://software.intel.com/en-us/articles/morphological-antialiasing-mlaa-sample.
// And I wrote a simple description in Chinese on the website.
// See https://www.bilibili.com/read/cv2269091.

namespace fl {
	namespace geom {
#ifdef ILL_SSE
		class MorphologicalAntialiasingAgent {
		private:
			static constexpr DWORD FlagH = 1 << 31;
			static constexpr const DWORD FlagV = 1 << 30;
			static constexpr const scalar UZ_Weight = 0.125f;
			static constexpr const scalar L_Weight = 0.25f;

			int height, width, size;
			DWORD* colors;
			void FindDiscontinuities();
			void ComparePixels(__m128i& vpix0, __m128i& vpix1, const DWORD flag);
			void ComputeH(DWORD* line);
			void ComputeV(DWORD* line);
			void BlendH_UZ(DWORD* line, int start, int end, int left_offset, int right_offset);
			void BlendH_L(DWORD* line, int start, int end, BOOL left_flat, int offset);
			void BlendV_UZ(DWORD* line, int start, int end, int top_offset, int bottom_offset);
			void BlendV_L(DWORD* line, int start, int end, BOOL top_flat, int offset);

		public:
			MorphologicalAntialiasingAgent() :colors(nullptr), height(0), width(0), size(0) {}
			void Execute(DWORD* src, int width, int height);
		};
#endif
	}
}