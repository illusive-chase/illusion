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

/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2013 Erwin Coumans  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/



#pragma once

#include "SConfig.h"

#if defined(DEBUG) || defined(_DEBUG)
#define ILL_DEBUG
#endif

#include "SLog.h"

#ifdef _WIN32

#if defined(__MINGW32__) || defined(__CYGWIN__) || (defined(_MSC_VER) && _MSC_VER < 1300)

#define ILL_INLINE inline
#define ILL_ATTRIBUTE_ALIGNED16(a) a
#define ILL_ATTRIBUTE_ALIGNED64(a) a
#define ILL_ATTRIBUTE_ALIGNED128(a) a
#else
#define ILL_HAS_ALIGNED_ALLOCATOR
#pragma warning(disable : 4324)  // disable padding warning
//			#pragma warning(disable:4530) // Disable the exception disable but used in MSCV Stl warning.
#pragma warning(disable : 4996)  //Turn off warnings about deprecated C routines
//			#pragma warning(disable:4786) // Disable the "debug name too long" warning

#define ILL_INLINE __forceinline
#define ILL_ATTRIBUTE_ALIGNED16(a) __declspec(align(16)) a
#define ILL_ATTRIBUTE_ALIGNED64(a) __declspec(align(64)) a
#define ILL_ATTRIBUTE_ALIGNED128(a) __declspec(align(128)) a

#if (defined(_WIN32) && (_MSC_VER) && _MSC_VER >= 1400) && (!defined(ILL_USE_DOUBLE_PRECISION)) && (defined(_M_IX86) || defined(_M_X64))
#if defined ILL_SSE
#include <emmintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>
#elif defined ILL_SSE_IN_API
#undef ILL_SSE_IN_API
#endif

#if defined ILL_SSE_IN_API
#ifdef _WIN32

#ifndef ILL_NAN
static int b3NanMask = 0x7F800001;
#define ILL_NAN (*(float *)&b3NanMask)
#endif

#ifndef ILL_INFINITY_MASK
static int b3InfinityMask = 0x7F800000;
#define ILL_INFINITY_MASK (*(float *)&b3InfinityMask)
#endif

inline __m128 operator+(const __m128 A, const __m128 B) {
	return _mm_add_ps(A, B);
}

inline __m128 operator-(const __m128 A, const __m128 B) {
	return _mm_sub_ps(A, B);
}

inline __m128 operator*(const __m128 A, const __m128 B) {
	return _mm_mul_ps(A, B);
}

#define illCastfTo128i(a) (_mm_castps_si128(a))
#define illCastfTo128d(a) (_mm_castps_pd(a))
#define illCastiTo128f(a) (_mm_castsi128_ps(a))
#define illCastdTo128f(a) (_mm_castpd_ps(a))
#define illCastdTo128i(a) (_mm_castpd_si128(a))
#define illAssign128(r0, r1, r2, r3) _mm_setr_ps(r0, r1, r2, r3)

#endif  //_WIN32
#endif  //ILL_SSE_IN_API


#elif defined ILL_SSE
#undef ILL_SSE
#endif

#endif  //__MINGW32__

#ifdef ILL_DEBUG
#ifdef _MSC_VER
#include <stdio.h>
#define ILLAssert(x)               \
	{                             \
		if (!(x))                 \
		{                         \
			ILLError(              \
				"Assert "__FILE__ \
				":%u (" #x ")\n", \
				__LINE__);        \
			__debugbreak();       \
		}                         \
	}
#else  //_MSC_VER
#include <assert.h>
#define ILLAssert assert
#endif  //_MSC_VER
#else
#define ILLAssert(x)
#endif

#else   //_WIN32

Failed!

#endif  //_WIN32

namespace fl {

#if defined(ILL_USE_DOUBLE_PRECISION)
	using scalar = double;
	//this number could be bigger in double precision
#define ILL_LARGE_FLOAT 1e30
#else
	using scalar = float;
	//keep ILL_LARGE_FLOAT * ILL_LARGE_FLOAT < FLT_MAX
#define ILL_LARGE_FLOAT 1e18f
#endif

#ifdef ILL_SSE
	using f4 = __m128;
#endif  //ILL_SSE

#define ILL_DECLARE_ALIGNED_ALLOCATOR                                                                  \
	ILL_INLINE void *operator new(size_t sizeInBytes) { return AlignedAlloc(sizeInBytes, 16); }   \
	ILL_INLINE void operator delete(void *ptr) { AlignedFree(ptr); }                              \
	ILL_INLINE void *operator new(size_t, void *ptr) { return ptr; }                                \
	ILL_INLINE void operator delete(void *, void *) {}                                              \
	ILL_INLINE void *operator new[](size_t sizeInBytes) { return AlignedAlloc(sizeInBytes, 16); } \
	ILL_INLINE void operator delete[](void *ptr) { AlignedFree(ptr); }                            \
	ILL_INLINE void *operator new[](size_t, void *ptr) { return ptr; }                              \
	ILL_INLINE void operator delete[](void *, void *) {}                                           

#if defined(ILL_USE_DOUBLE_PRECISION)

	ILL_INLINE scalar illSqrt(scalar x) { return sqrt(x); }
	ILL_INLINE scalar illFabs(scalar x) { return fabs(x); }
	ILL_INLINE scalar illCos(scalar x) { return cos(x); }
	ILL_INLINE scalar illSin(scalar x) { return sin(x); }
	ILL_INLINE scalar illTan(scalar x) { return tan(x); }
	ILL_INLINE scalar illArccos(scalar x) {
		if (x < scalar(-1)) x = scalar(-1);
		if (x > scalar(1)) x = scalar(1);
		return acos(x);
	}
	ILL_INLINE scalar illArcsin(scalar x) {
		if (x < scalar(-1)) x = scalar(-1);
		if (x > scalar(1)) x = scalar(1);
		return asin(x);
	}
	ILL_INLINE scalar illArctan(scalar x) { return atan(x); }
	ILL_INLINE scalar illArctan2(scalar x, scalar y) { return atan2(x, y); }
	ILL_INLINE scalar illExp(scalar x) { return exp(x); }
	ILL_INLINE scalar illLog(scalar x) { return log(x); }
	ILL_INLINE scalar illPow(scalar x, scalar y) { return pow(x, y); }
	ILL_INLINE scalar illFmod(scalar x, scalar y) { return fmod(x, y); }

#else

	ILL_INLINE scalar illSqrt(scalar y) {
#ifdef USE_APPROXIMATION
		double x, z, tempf;
		unsigned long *tfptr = ((unsigned long *)&tempf) + 1;

		tempf = y;
		*tfptr = (0xbfcdd90a - *tfptr) >> 1; /* estimate of 1/sqrt(y) */
		x = tempf;
		z = y * scalar(0.5);
		x = (scalar(1.5) * x) - (x * x) * (x * z); /* iteration formula     */
		x = (scalar(1.5) * x) - (x * x) * (x * z);
		x = (scalar(1.5) * x) - (x * x) * (x * z);
		x = (scalar(1.5) * x) - (x * x) * (x * z);
		x = (scalar(1.5) * x) - (x * x) * (x * z);
		return x * y;
#else
		return sqrtf(y);
#endif
	}
	ILL_INLINE scalar illFabs(scalar x) { return fabsf(x); }
	ILL_INLINE scalar illCos(scalar x) { return cosf(x); }
	ILL_INLINE scalar illSin(scalar x) { return sinf(x); }
	ILL_INLINE scalar illTan(scalar x) { return tanf(x); }
	ILL_INLINE scalar illArccos(scalar x) {
		if (x < scalar(-1))
			x = scalar(-1);
		if (x > scalar(1))
			x = scalar(1);
		return acosf(x);
	}
	ILL_INLINE scalar illArcsin(scalar x) {
		if (x < scalar(-1))
			x = scalar(-1);
		if (x > scalar(1))
			x = scalar(1);
		return asinf(x);
	}
	ILL_INLINE scalar illArctan(scalar x) { return atanf(x); }
	ILL_INLINE scalar illArctan2(scalar x, scalar y) { return atan2f(x, y); }
	ILL_INLINE scalar illExp(scalar x) { return expf(x); }
	ILL_INLINE scalar illLog(scalar x) { return logf(x); }
	ILL_INLINE scalar illPow(scalar x, scalar y) { return powf(x, y); }
	ILL_INLINE scalar illFmod(scalar x, scalar y) { return fmodf(x, y); }

#endif

	ILL_INLINE scalar illArctan2Fast(scalar y, scalar x) {
		scalar coeff_1 = PI / 4.0f;
		scalar coeff_2 = 3.0f * coeff_1;
		scalar abs_y = illFabs(y);
		scalar angle;
		if (x >= 0.0f) {
			scalar r = (x - abs_y) / (x + abs_y);
			angle = coeff_1 - coeff_1 * r;
		} else {
			scalar r = (x + abs_y) / (abs_y - x);
			angle = coeff_2 - coeff_1 * r;
		}
		return (y < 0.0f) ? -angle : angle;
	}

	ILL_INLINE bool Fz(scalar x) { return illFabs(x) < EPS; }

	ILL_INLINE bool Eq(scalar a, scalar eps) {
		return (((a) <= eps) && !((a) < -eps));
	}
	ILL_INLINE bool Ge(scalar a, scalar eps) {
		return (!((a) <= eps));
	}

	ILL_INLINE int IsNegative(scalar x) {
		return x < scalar(0.0) ? 1 : 0;
	}

	ILL_INLINE scalar Radians(scalar x) { return x * (PI / scalar(180.0)); }
	ILL_INLINE scalar Degrees(scalar x) { return x * (scalar(180.0) / PI); }

	ILL_INLINE scalar Fsel(scalar a, scalar b, scalar c) { return a >= 0 ? b : c; }

	ILL_INLINE bool MachineIsLittleEndian() {
		long int i = 1;
		const char *p = (const char *)&i;
		if (p[0] == 1)  // Lowest address contains the least significant byte
			return true;
		else
			return false;
	}


	template <typename T>
	ILL_INLINE void illSwap(T &a, T &b) {
		T tmp = a;
		a = b;
		b = tmp;
	}


	// returns normalized value in range [-B3_PI, B3_PI]
	ILL_INLINE scalar NormalizeAngle(scalar angleInRadians) {
		angleInRadians = illFmod(angleInRadians, 2 * PI);
		if (angleInRadians < -PI) {
			return angleInRadians + 2 * PI;
		} else if (angleInRadians > PI) {
			return angleInRadians - 2 * PI;
		} else {
			return angleInRadians;
		}
	}

	//rudimentary class to provide type info
	struct TypedObject {
		TypedObject(int objectType)
			: m_objectType(objectType) {
		}
		int m_objectType;
		inline int getObjectType() const {
			return m_objectType;
		}
	};

	//align a pointer to the provided alignment, upwards
	template <typename T>
	T *AlignPointer(T *unalignedPtr, size_t alignment) {
		struct ConvertPointerSizeT {
			union {
				T *ptr;
				size_t integer;
			};
		};
		ConvertPointerSizeT converter;

		const size_t bit_mask = ~(alignment - 1);
		converter.ptr = unalignedPtr;
		converter.integer += alignment - 1;
		converter.integer &= bit_mask;
		return converter.ptr;
	}

	template <class T>
	ILL_INLINE const T& illMin(const T& a, const T& b) {
		return a < b ? a : b;
	}

	template <class T>
	ILL_INLINE const T& illMax(const T& a, const T& b) {
		return a > b ? a : b;
	}

	template <class T>
	ILL_INLINE const T& illClamped(const T& a, const T& lb, const T& ub) {
		return a < lb ? lb : (ub < a ? ub : a);
	}

	template <class T>
	ILL_INLINE void illSetMin(T& a, const T& b) {
		if (b < a) {
			a = b;
		}
	}

	template <class T>
	ILL_INLINE void illSetMax(T& a, const T& b) {
		if (a < b) {
			a = b;
		}
	}

	template <class T>
	ILL_INLINE void illClamp(T& a, const T& lb, const T& ub) {
		if (a < lb) {
			a = lb;
		} else if (ub < a) {
			a = ub;
		}
	}

}