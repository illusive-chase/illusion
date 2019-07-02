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
#include "SMLAAA.h"


#ifdef ILL_SSE
void fl::geom::MorphologicalAntialiasingAgent::FindDiscontinuities() {
	for (int irow = 0; irow < height; irow += 4) {
		int width_offset = irow * width;
		for (int icol = 0; icol < width; icol += 4) {
			__m128i *ptr = reinterpret_cast<__m128i*>((DWORD*)colors + width_offset + icol);
			int width128 = width >> 2;

			__m128i mask = _mm_set1_epi32(0x00FFFFFF);
			__m128i vpix0 = _mm_and_si128(mask, _mm_load_si128(ptr));
			__m128i vpix1 = _mm_and_si128(mask, _mm_load_si128(ptr + width128));
			__m128i vpix2 = _mm_and_si128(mask, _mm_load_si128(ptr + width128 * 2));
			__m128i vpix3 = _mm_and_si128(mask, _mm_load_si128(ptr + width128 * 3));
			__m128i vpix4 = (irow == height - 4) ? vpix3 : _mm_and_si128(mask, _mm_load_si128(ptr + width));

			ComparePixels(vpix0, vpix1, FlagH);
			ComparePixels(vpix1, vpix2, FlagH);
			ComparePixels(vpix2, vpix3, FlagH);
			ComparePixels(vpix3, vpix4, FlagH);

			_MM_TRANSPOSE4_PS(
				reinterpret_cast<__m128&>(vpix0),
				reinterpret_cast<__m128&>(vpix1),
				reinterpret_cast<__m128&>(vpix2),
				reinterpret_cast<__m128&>(vpix3));

			vpix4 = (icol == width - 4) ? vpix3 : _mm_setr_epi32(
				*reinterpret_cast<DWORD*>(ptr + 1),
				*reinterpret_cast<DWORD*>(ptr + width128 + 1),
				*reinterpret_cast<DWORD*>(ptr + width128 * 2 + 1),
				*reinterpret_cast<DWORD*>(ptr + width128 * 3 + 1));

			ComparePixels(vpix0, vpix1, FlagV);
			ComparePixels(vpix1, vpix2, FlagV);
			ComparePixels(vpix2, vpix3, FlagV);
			ComparePixels(vpix3, vpix4, FlagV);

			_MM_TRANSPOSE4_PS(
				reinterpret_cast<__m128&>(vpix0),
				reinterpret_cast<__m128&>(vpix1),
				reinterpret_cast<__m128&>(vpix2),
				reinterpret_cast<__m128&>(vpix3));

			_mm_store_si128(ptr, vpix0);
			_mm_store_si128(ptr + width128, vpix1);
			_mm_store_si128(ptr + width128 * 2, vpix2);
			_mm_store_si128(ptr + width128 * 3, vpix3);
		}
	}
}

inline void fl::geom::MorphologicalAntialiasingAgent::ComparePixels(__m128i& vpix0, __m128i& vpix1, const DWORD flag) {
	__m128i vdiff = _mm_sub_epi8(_mm_max_epu8(vpix0, vpix1), _mm_min_epu8(vpix0, vpix1));
	__m128i mask = _mm_set1_epi32(0x00F0F0F0);
	vdiff = _mm_and_si128(vdiff, mask);
	__m128i selector = _mm_cmpeq_epi32(vdiff, _mm_setzero_si128());
	__m128i vflag = _mm_set1_epi32(flag);
	vflag = _mm_andnot_si128(selector, vflag);
	vpix0 = _mm_or_si128(vpix0, vflag);
}

void fl::geom::MorphologicalAntialiasingAgent::ComputeH(DWORD * line) {
	int p = 0;
	for (;;) {
		int left_secondary_offset = -1, right_secondary_offset = -1;
		int start = -1, end = 0;
		for (; p < width; ++p) {
			if (line[p] & FlagH) {
				start = p;
				break;
			}
		}
		if (!~start) return;
		for (; p < width; ++p) {
			if (!(line[p] & FlagH)) {
				end = p;
				break;
			}
		}
		if (!end) end = p;

		//blend
		if (start) {
			if (line[start - 1] & FlagV) left_secondary_offset = 0;
			else if (line[start - 1 + width] & FlagV) left_secondary_offset = 1;
		}
		if (line[end - 1] & FlagV) right_secondary_offset = 0;
		else if (line[end - 1 + width] & FlagV) right_secondary_offset = 1;

		if (~left_secondary_offset) {
			if (~right_secondary_offset) BlendH_UZ(line, start, end, left_secondary_offset, right_secondary_offset);
			else BlendH_L(line, start, end, 0, left_secondary_offset);
		} else if (~right_secondary_offset) BlendH_L(line, start, end, 1, right_secondary_offset);
	}
}

void fl::geom::MorphologicalAntialiasingAgent::ComputeV(DWORD * line) {
	int p = 0;
	for (;;) {
		int top_secondary_offset = -1, bottom_secondary_offset = -1;
		int start = -1, end = 0;
		for (; p < size; p += width) {
			if (line[p] & FlagV) {
				start = p;
				break;
			}
		}
		if (!~start) return;
		for (; p < size; p += width) {
			if (!(line[p] & FlagV)) {
				end = p;
				break;
			}
		}
		if (!end) end = p;

		//blend
		if (start) {
			if (line[start - width] & FlagH) top_secondary_offset = 0;
			else if (line[start - width + 1] & FlagH) top_secondary_offset = 1;
		}
		if (line[end - width] & FlagH) bottom_secondary_offset = 0;
		else if (line[end - width + 1] & FlagH) bottom_secondary_offset = 1;

		if (~bottom_secondary_offset) {
			if (~top_secondary_offset) BlendV_UZ(line, start, end, top_secondary_offset, bottom_secondary_offset);
			else BlendV_L(line, start, end, 1, bottom_secondary_offset);
		} else if (~top_secondary_offset) BlendV_L(line, start, end, 0, top_secondary_offset);
	}
}

inline void fl::geom::MorphologicalAntialiasingAgent::BlendH_UZ(DWORD * line, int start, int end, int left_offset, int right_offset) {
	int len = end - start;
	if (len & 1) {
		len = (len + 1) >> 1;
		DWORD* write1 = line + start + len - 1 + left_offset * width;
		left_offset = left_offset ? -width : width;

		DWORD* write2 = line + end - len + right_offset * width;
		right_offset = right_offset ? -width : width;

		scalar w = UZ_Weight / (end - start);
		scalar wi = w;

		for (int i = 0; i < len;) {
			MIX32(*write1, wi, *(write1 + left_offset));
			MIX32(*write2, wi, *(write2 + right_offset));
			--write1;
			++write2;
			wi = (++i << 3) * w;
		}
	} else {
		len >>= 1;
		DWORD* write1 = line + start + len - 1 + left_offset * width;
		left_offset = left_offset ? -width : width;

		DWORD* write2 = line + end - len + right_offset * width;
		right_offset = right_offset ? -width : width;

		scalar w = 2.0f * UZ_Weight / len;

		for (int i = 0; i < len; ++i) {
			MIX32(*write1, ((i << 1) | 1) * w, *(write1 + left_offset));
			MIX32(*write2, ((i << 1) | 1) * w, *(write2 + right_offset));
			--write1;
			++write2;
		}
	}
}

inline void fl::geom::MorphologicalAntialiasingAgent::BlendH_L(DWORD * line, int start, int end, BOOL left_flat, int offset) {
	DWORD* write = line + start + offset * width;
	offset = offset ? -width : width;
	int len = end - start;
	scalar w = L_Weight / len;
	if (left_flat) {
		for (int i = 0; i < len; ++i) {
			MIX32(*write, ((i << 1) | 1) * w, *(write + offset));
			++write;
		}
	} else {
		for (int i = len - 1; i >= 0; --i) {
			MIX32(*write, ((i << 1) | 1) * w, *(write + offset));
			++write;
		}
	}
}

inline void fl::geom::MorphologicalAntialiasingAgent::BlendV_UZ(DWORD * line, int start, int end, int top_offset, int bottom_offset) {
	int len = (end - start) / width;
	if (len & 1) {
		len = (len + 1) >> 1;
		DWORD* write1 = line + start + (len - 1) * width + top_offset;
		top_offset = top_offset ? -1 : 1;

		DWORD* write2, *mix2;
		mix2 = write2 = line + end - len * width + bottom_offset;
		bottom_offset = bottom_offset ? -1 : 1;

		scalar w = UZ_Weight / ((end - start) / width);
		scalar wi = w;

		for (int i = 0; i < len;) {
			MIX32(*write1, wi, *(write1 + top_offset));
			MIX32(*write2, wi, *(write2 + bottom_offset));
			write1 -= width;
			write2 += width;
			wi = (++i << 3) * w;
		}
	} else {
		len >>= 1;
		DWORD* write1 = line + start + (len - 1) * width + top_offset;
		top_offset = top_offset ? -1 : 1;

		DWORD* write2, *mix2;
		mix2 = write2 = line + end - len * width + bottom_offset;
		bottom_offset = bottom_offset ? -1 : 1;

		scalar w = 2.0f * UZ_Weight / len;

		for (int i = 0; i < len; ++i) {
			MIX32(*write1, ((i << 1) | 1) * w, *(write1 + top_offset));
			MIX32(*write2, ((i << 1) | 1) * w, *(write2 + bottom_offset));
			write1 -= width;
			write2 += width;
		}
	}
}

inline void fl::geom::MorphologicalAntialiasingAgent::BlendV_L(DWORD * line, int start, int end, BOOL top_flat, int offset) {
	DWORD* write = line + start + offset;
	offset = offset ? -1 : 1;
	int len = (end - start) / width;
	scalar w = L_Weight / len;
	if (top_flat) {
		for (int i = 0; i < len; ++i) {
			MIX32(*write, ((i << 1) | 1) * w, *(write + offset));
			write += width;
		}
	} else {
		for (int i = len - 1; i >= 0; --i) {
			MIX32(*write, ((i << 1) | 1) * w, *(write + offset));
			write += width;
		}
	}
}

void fl::geom::MorphologicalAntialiasingAgent::Execute(DWORD * src, int width, int height) {
	colors = src;
	this->width = width;
	this->height = height;
	size = width * height;
	FindDiscontinuities();
	for (int i = 0; i < height; ++i) ComputeH(colors + i * width);
	for (int i = 0; i < width; ++i) ComputeV(colors + i);
}
#endif