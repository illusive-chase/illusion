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
// Provide 3D math, including matrix, vector, fixed point number, interpolation, coloring, etc.
// The implementation of vector partially draws on the vector of the engine bullet.

#include "../top_element/SAlignedAllocator.h"
#include "../display/SColor.h"
#include <random>

namespace fl {
	namespace geom {


		class Rad {
		public:
			scalar s, c;
			Rad(scalar rad) :s(illSin(rad)), c(illCos(rad)) {}
			Rad(scalar s, scalar c) :s(s), c(c) {}

			Rad operator -() const { return Rad(-s, c); } // as cos(-rad) equals cos(rad)
		};

		// unfinished
		/*class Matrix3D {
		public:
			scalar value[4][4];
			Matrix3D() {
				memset(value, 0, sizeof value);
			}
			Matrix3D(const std::initializer_list<scalar>& list) {
				memset(value, 0, sizeof value);
				auto p = list.begin();
				for (int i = 0; i < 16 && p != list.end(); p++, i++) {
					value[0][i] = *p;
				}
			}
		};*/

		// It maps UV coordinates to a 32-bit integer.
		ILL_INLINE DWORD UV(DWORD u, DWORD v) {
			return u << 16 | v;
		}

		// It multiplies the color RGB value by a weight.
		ILL_INLINE BYTE MIX(BYTE a, scalar w) {
			int c = int(a * w);
			return c > 0xFF ? 0xFF : BYTE(c);
		}

		// Alpha Blend
		// It mixes two RGB according to the weight, and assigns the mixed value to the first parameter.
		// ATTENTION:
		// 1. w_e must be in range [0, 1].
		// 2. The highest byte does not change.
		ILL_INLINE void MIX32(DWORD& c, scalar w_e, DWORD e) {
			scalar w_c = scalar(1) - w_e;
#ifdef ILL_SSE
			__m128i c1 = _mm_cvtsi32_si128(c);
			__m128i c2 = _mm_cvtsi32_si128(e);
			const __m128i zero = _mm_setzero_si128();
			c1 = _mm_unpacklo_epi16(_mm_unpacklo_epi8(c1, zero), zero);
			c2 = _mm_unpacklo_epi16(_mm_unpacklo_epi8(c2, zero), zero);
			__m128 f1 = _mm_cvtepi32_ps(c1);
			__m128 f2 = _mm_cvtepi32_ps(c2);
			f1 = _mm_mul_ps(f1, _mm_set1_ps(w_c));
			f2 = _mm_mul_ps(f2, _mm_set1_ps(w_e));
			f1 = _mm_add_ps(f1, f2);
			__m128i c0 = _mm_cvtps_epi32(f1);
			c0 = _mm_packs_epi32(c0, zero);
			c0 = _mm_packus_epi16(c0, zero);
			c = ((DWORD)_mm_cvtsi128_si32(c0) & 0x00FFFFFF) | (c & 0xFF000000);
#else
			int r = int(((BYTE)(c >> 16)) * w_c + ((BYTE)(e >> 16)) * w_e);
			int g = int(((BYTE)(c >> 8)) * w_c + ((BYTE)(e >> 8)) * w_e);
			int b = int(((BYTE)(c)) * w_c + ((BYTE)(e)) * w_e);
			c = (RGB3D(r, g, b) & 0x00FFFFFF) | (c & 0xFF000000);
#endif
		}

		



		// Here borrowed some of the vector of the engine bullet.
		ILL_ATTRIBUTE_ALIGNED16(class) Vector3D {
		public:
#if defined(ILL_SSE)
			union {
				f4 m_vec128;
				scalar m_floats[4];
				struct { scalar x, y, z, w; };
			};

			Vector3D(f4 m_vec128) :m_vec128(m_vec128) {}

#else
			union {
				scalar m_floats[4];
				struct { scalar x, y, z, w; };
			};
#endif
			ILL_DECLARE_ALIGNED_ALLOCATOR

			Vector3D() :x(0), y(0), z(0), w(0) {}

			Vector3D(int x, int y, int z) :x(scalar(x)), y(scalar(y)), z(scalar(z)), w(0) {}

			Vector3D(scalar x, scalar y, scalar z, scalar w = 0) :x(x), y(y), z(z), w(w) {}

			Vector3D(const scalar(&arr)[4]) :x(arr[0]), y(arr[1]), z(arr[2]), w(arr[3]) {}

#ifdef COUNT_VECTOR_CONSTRUCT
			Vector3D(const Vector3D& rhs) :m_vec128(rhs.m_vec128) { ++copy_cnt(); }

			Vector3D(Vector3D&& rhs) noexcept :x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) { ++move_cnt(); }

			Vector3D& operator =(const Vector3D& rhs) { ++copy_cnt(); m_vec128 = rhs.m_vec128; return *this; }

			Vector3D& operator =(Vector3D&& rhs) noexcept { ++move_cnt(); x = rhs.x, y = rhs.y, z = rhs.z, w = rhs.w; return *this; }

			static uint& move_cnt() { static uint cnt = 0; return cnt; }
			static uint& copy_cnt() { static uint cnt = 0; return cnt; }
#endif

			ILL_INLINE Vector3D operator -() const { return Vector3D(-x, -y, -z, -w); }

			ILL_INLINE Vector3D& operator +=(const Vector3D& tar) {
#ifdef ILL_SSE_IN_API
				m_vec128 = _mm_add_ps(m_vec128, tar.m_vec128);
#else
				x += tar.x; y += tar.y; z += tar.z; w += tar.w;
#endif
				return *this;
			}

			ILL_INLINE Vector3D operator +(const Vector3D& tar) const { 
#ifdef ILL_SSE_IN_API
				return Vector3D(_mm_add_ps(m_vec128, tar.m_vec128));
#else
				return Vector3D(x + tar.x, y + tar.y, z + tar.z, w + tar.w);
#endif
			}

			ILL_INLINE Vector3D& operator -=(const Vector3D& tar) {
#ifdef ILL_SSE_IN_API
				m_vec128 = _mm_sub_ps(m_vec128, tar.m_vec128);
#else
				x -= tar.x; y -= tar.y; z -= tar.z;  w -= tar.w;
#endif
				return *this;
			}

			ILL_INLINE Vector3D operator -(const Vector3D& tar)  const { 
#ifdef ILL_SSE_IN_API
				return Vector3D(_mm_sub_ps(m_vec128, tar.m_vec128));
#else
				return Vector3D(x - tar.x, y - tar.y, z - tar.z, w - tar.w);
#endif
			}

			ILL_INLINE Vector3D& operator &=(const Vector3D& tar) {
#ifdef ILL_SSE_IN_API
				m_vec128 = _mm_mul_ps(m_vec128, tar.m_vec128);
#else
				x *= tar.x; y *= tar.y; z *= tar.z;  w *= tar.w;
#endif
				return *this;
			}

			ILL_INLINE Vector3D operator &(const Vector3D& tar)  const {
#ifdef ILL_SSE_IN_API
				return Vector3D(_mm_mul_ps(m_vec128, tar.m_vec128));
#else
				return Vector3D(x * tar.x, y * tar.y, z * tar.z, w * tar.w);
#endif
			}

			ILL_INLINE Vector3D& operator *=(const scalar& k) {
#ifdef ILL_SSE_IN_API
				__m128 vs = _mm_load_ss(&k);  //	(S 0 0 0)
				vs = ill_pshufd_ps(vs, 0x0);  //	(S S S S)  0x80 = _MM_SHUFFLE(0, 0, 0, 0)
				m_vec128 = _mm_mul_ps(m_vec128, vs);
#else
				x *= k; y *= k; z *= k; w *= k;
#endif
				return *this;
			}

			ILL_INLINE Vector3D& operator /=(scalar k) { return *this *= (scalar(1) / k); }

			ILL_INLINE scalar operator *(const Vector3D& tar) const { 
#ifdef ILL_SSE_IN_API
				__m128 vd = _mm_mul_ps(m_vec128, tar.m_vec128);
				__m128 z = _mm_movehl_ps(vd, vd);
				__m128 y = _mm_shuffle_ps(vd, vd, 0x55);  //   0x55 = _MM_SHUFFLE(1, 1, 1, 1)
				vd = _mm_add_ss(vd, y);
				vd = _mm_add_ss(vd, z);
				return _mm_cvtss_f32(vd);
#else
				return x * tar.x + y * tar.y + z * tar.z + w * tar.w;
#endif
			}

			ILL_INLINE Vector3D operator /(const Vector3D& tar) const {
#ifdef ILL_SSE_IN_API
				return Vector3D(_mm_div_ps(m_vec128, tar.m_vec128));
#else
				return Vector3D(x / tar.x, y / tar.y, z / tar.z, w ? (w / tar.w) : w);
#endif
			}

			ILL_INLINE Vector3D operator *(const scalar& k) const {
#ifdef ILL_SSE_IN_API
				__m128 vs = _mm_load_ss(&k);  //	(S 0 0 0)
				vs = ill_pshufd_ps(vs, 0x0);  //	(S S S S)  0x0 = _MM_SHUFFLE(0, 0, 0, 0)
				return Vector3D(_mm_mul_ps(m_vec128, vs));
#else
				return Vector3D(x * k, y * k, z * k, w * k);
#endif
			}

			ILL_INLINE scalar mod2() const { return (*this) * (*this); }

			ILL_INLINE scalar mod() const { return illSqrt(mod2()); }

			ILL_INLINE Vector3D& normalize() {
#ifdef ILL_SSE_IN_API
				__m128 vd = _mm_mul_ps(m_vec128, m_vec128);
				__m128 z = _mm_movehl_ps(vd, vd);
				__m128 y = _mm_shuffle_ps(vd, vd, 0x55);  //  //   0x55 = _MM_SHUFFLE(1, 1, 1, 1)
				vd = _mm_add_ss(vd, y);
				vd = _mm_add_ss(vd, z);

				// NR step 1/sqrt(x) - vd is x, y is output
				y = _mm_rsqrt_ss(vd);  // estimate

				//  one step NR
				z = illv1_5;
				vd = _mm_mul_ss(vd, illvHalf);  // vd * 0.5
				//x2 = vd;
				vd = _mm_mul_ss(vd, y);  // vd * 0.5 * y0
				vd = _mm_mul_ss(vd, y);  // vd * 0.5 * y0 * y0
				z = _mm_sub_ss(z, vd);   // 1.5 - vd * 0.5 * y0 * y0

				y = _mm_mul_ss(y, z);  // y0 * (1.5 - vd * 0.5 * y0 * y0)

				y = ill_splat_ps(y, 0x0);
				m_vec128 = _mm_mul_ps(m_vec128, y);
#else
				scalar k = mod();
				x /= k;	y /= k;	z /= k; w /= k;
#endif
				return *this;
			}

			ILL_INLINE bool operator ==(const Vector3D& cp) const {
				return Eq(x - cp.x, EPS) && Eq(y - cp.y, EPS) && Eq(z - cp.z, EPS);
			}

			ILL_INLINE Vector3D& rotateX(const Rad& rad) {
				scalar temp = rad.c * y + rad.s * z;
				z = rad.c * z - rad.s * y;
				y = temp;
				return *this;
			}

			ILL_INLINE Vector3D& rotateY(const Rad& rad) {
				scalar temp = rad.c * x - rad.s * z;
				z = rad.c * z + rad.s * x;
				x = temp;
				return *this;
			}

			ILL_INLINE Vector3D& rotateZ(const Rad& rad) {
				scalar temp = rad.c * x + rad.s * y;
				y = rad.c * y - rad.s * x;
				x = temp;
				return *this;
			}

			ILL_INLINE operator float*() {
				return m_floats;
			}

			ILL_INLINE operator const float*() const {
				return m_floats;
			}

		};


		// The name Shadee is related to Shader(like employer and employee).
		// Class Shadee carrys some important data of a point, used for rendering.
		// The variables x,y are the x,y coordinates in screen space.
		// The variables u,v are the UV coordinates multiplied by z.
		// The variables r,g,b are the RGB value of the light color multiplied by z.
		// The variable z is the reciprocal of the z depth in 3D space.
	    // ATTENTION: It is z depth, NOT z coordinates. Z depth of the point means the depth of the point in screen space.
		// The u,v,r,g,b,z values are different from the actual because of the need for perspective correction interpolation.
		// The above attributes belong to one point in 3D space.
		ILL_ATTRIBUTE_ALIGNED16(class) Shadee {
		public:
			ILL_DECLARE_ALIGNED_ALLOCATOR

#ifdef ILL_SSE
			union {
				struct { scalar x, y, u, v, b, g, r, z; };
				struct { f4 m_xyuv, m_bgrz; };
				struct { scalar floats[8]; };
			};
#else
			union {
				struct { scalar x, y, u, v, b, g, r, z; };
				struct { scalar floats[8]; };
			};
#endif
			Shadee() {}

			// Interpolation.
			// It constructs a Shadee of which y equals y0 and 
			// of which (x,y) is on the line connecting (va->x, va->y) and (vb->x, vb->y).
			Shadee(Shadee* va, Shadee* vb, scalar y0) {
				scalar ILL_ATTRIBUTE_ALIGNED16(t) = (y0 - va->y) / (vb->y - va->y);
				scalar ILL_ATTRIBUTE_ALIGNED16(rt) = scalar(1) - t;
#ifdef ILL_SSE_IN_API
				{
					__m128 m_t = _mm_load1_ps(&t);
					__m128 m_rt = _mm_load1_ps(&rt);
					m_xyuv = _mm_add_ps(_mm_mul_ps(va->m_xyuv, m_rt), _mm_mul_ps(vb->m_xyuv, m_t));
					m_bgrz = _mm_add_ps(_mm_mul_ps(va->m_bgrz, m_rt), _mm_mul_ps(vb->m_bgrz, m_t));
				}
#else
				{
					x = va->x * rt + vb->x * t;
					y = va->y * rt + vb->y * t;
					z = va->z * rt + vb->z * t;
					r = va->r * rt + vb->r * t;
					g = va->g * rt + vb->g * t;
					b = va->b * rt + vb->b * t;
					u = va->u * rt + vb->u * t;
					v = va->v * rt + vb->v * t;
				}
#endif
			}

			// It is just for convenience.
			// See the function Stage3DImpl::project, which is the only place this function is called.
			ILL_INLINE void set(scalar x, scalar y, const Vector3D& color, scalar z) {
				this->x = x;
				this->y = y;
				r = color.x;
				g = color.y;
				b = color.z;
				this->z = z;
			}

			// It returns true if and only if the three points respectively presented by a,b,c in the screen space
			// are arranged clockwise.
			static bool clockwise(const Shadee& a, const Shadee& b, const Shadee& c) {
				return (c.x - a.x)*b.y + (b.x - c.x)*a.y + (a.x - b.x)*c.y < scalar(0);
			}
		};

		// Class LerpY is a helper class that interpolates along the y-axis. 
		// Class LerpY carrys the same data as that class Shadee carrys.
		// In fact, it can be considered a Shadee itself and it is like a sliding Shadee on a line between two Shadee.
		// Every time it slides, y will increase by 1.
		// For an attribute p, dp represents the amount of change in attribute p after each slide.
		// Obviously, dy must be 1.
		ILL_ATTRIBUTE_ALIGNED16(class) LerpY {
		public:
			ILL_DECLARE_ALIGNED_ALLOCATOR

#ifdef ILL_SSE
			union {
				struct { scalar x, y, u, v, b, g, r, z, dx, dy, du, dv, dr, dg, db, dz; };
				struct { f4 m_xyuv, m_bgrz, m_dxyuv, m_drgbz; };
				struct { scalar floats[16]; };
			};
#else
			union {
				struct { scalar x, y, u, v, b, g, r, z, dx, dy, du, dv, dr, dg, db, dz; };
				struct { scalar floats[16]; };
			};
#endif
			// The constructor is used to determine on the line connecting which two Shadee it is sliding.
			// The parameter 'step' indicates the amount of change in y (namely dy) per slide (default is 1).
			// The silding starts from 'sa'.
			ILL_INLINE LerpY(const Shadee& sa, const Shadee& sb, const scalar& step);

			// The constructor is used to determine on the line connecting which two Shadee it is sliding.
			// The dy is equal to 1 by default.
			// The silding starts from 'sa'.
			ILL_INLINE LerpY(const Shadee& sa, const Shadee& sb);

			// The parameter 'sample_y' is related to the sampling at the time of rendering.
			// See Stage3DImpl::drawTriangle and Stage3DImpl::drawTriangleMSAA in Stage3DImpl.cpp.
			ILL_INLINE void start(int ay, scalar sample_y) { move(ay - y + sample_y); }

			// Do a slide.
			ILL_INLINE void move() {
#ifdef ILL_SSE_IN_API
				m_bgrz = _mm_add_ps(m_bgrz, m_drgbz);
				m_xyuv = _mm_add_ps(m_xyuv, m_dxyuv);
#else
				x += dx;

				// Actually, if ILL_SSE_IN_API is not defined, dy may be not equal to 1.
				// This is because I am too lazy to design carefully.
				// So I just use 1 instead of dy here.
				y += scalar(1);

				z += dz;
				u += du;
				v += dv;
				r += dr;
				b += db;
				g += dg;
#endif
			}

			// Do a partial slide.
			// For example, when 'step' is equal to 0.25, it means sliding a quarter step.
			ILL_INLINE void move(const scalar& step) {
#ifdef ILL_SSE_IN_API
				f4 m_step = _mm_load_ps1(&step);
				m_bgrz = _mm_add_ps(m_bgrz, _mm_mul_ps(m_drgbz, m_step));
				m_xyuv = _mm_add_ps(m_xyuv, _mm_mul_ps(m_dxyuv, m_step));
#else
				x += dx * step;

				// Actually, if ILL_SSE_IN_API is not defined, dy may be not equal to 1.
				// This is because I am too lazy to design carefully.
				// So I just use step instead of dy*step here.
				y += step;

				z += dz * step;
				u += du * step;
				v += dv * step;
				r += dr * step;
				b += db * step;
				g += dg * step;
#endif
			}

			// Reset it to the starting position.
			// The parameter 'sa' MUST be the same Shadee as the 'sa' in constructor.
			ILL_INLINE void reset(const Shadee& sa) {
#ifdef ILL_SSE
				m_xyuv = sa.m_xyuv;
				m_bgrz = sa.m_bgrz;
#else
				x = sa.x;
				y = sa.y;
				z = sa.z;
				u = sa.u;
				v = sa.v;
				r = sa.r;
				g = sa.g;
				b = sa.b;
#endif
			}
		};

		// Class LerpX is a helper class that interpolates along the x-axis. 
		// Class LerpX carrys the same data as that class Shadee carrys.
		// In fact, it can be considered a Shadee itself and it is like 
		// a sliding Shadee on a line between two Shadee(actually two LerpY).
		// Every time it slides, x will increase by 1.
		// For an attribute p, dp represents the amount of change in attribute p after each slide.
		// Obviously, dx must be 1.
		ILL_ATTRIBUTE_ALIGNED16(class) LerpX {
		public:
			ILL_DECLARE_ALIGNED_ALLOCATOR

#ifdef ILL_SSE
			union {
				struct { scalar x, y, u, v, b, g, r, z, dx, dy, du, dv, dr, dg, db, dz; };
				struct { f4 m_xyuv, m_bgrz, m_dxyuv, m_drgbz; };
				struct { scalar floats[16]; };
			};
#else
			union {
				struct { scalar x, y, u, v, b, g, r, z, dx, dy, du, dv, dr, dg, db, dz; };
				struct { scalar floats[16]; };
			};
#endif
			// The constructor is used to determine on the line connecting which two Shadee(actually LerpY) it is sliding.
			// The dx is equal to 1 by default.
			// The silding starts from 'sa'.
			ILL_INLINE LerpX(const LerpY& sa, const LerpY& sb);

			ILL_INLINE void start(int ax, scalar sample_x) { move(ax - x + sample_x); }

			// Do a slide.
			ILL_INLINE void move() {
#ifdef ILL_SSE_IN_API
				m_bgrz = _mm_add_ps(m_bgrz, m_drgbz);
				m_xyuv = _mm_add_ps(m_xyuv, m_dxyuv);
#else
				// Actually, if ILL_SSE_IN_API is not defined, dx may be not equal to 1.
				// This is because I am too lazy to design carefully.
				// So I just use 1 instead of dx here.
				x += scalar(1);

				y += dy;
				z += dz;
				u += du;
				v += dv;
				r += dr;
				b += db;
				g += dg;
#endif
			}

			// Do a partial slide.
			// For example, when 'step' is equal to 0.25, it means sliding a quarter step.
			ILL_INLINE void move(const scalar& step) {
#ifdef ILL_SSE_IN_API
				f4 m_step = _mm_load_ps1(&step);
				m_bgrz = _mm_add_ps(m_bgrz, _mm_mul_ps(m_drgbz, m_step));
				m_xyuv = _mm_add_ps(m_xyuv, _mm_mul_ps(m_dxyuv, m_step));
#else
				// Actually, if ILL_SSE_IN_API is not defined, dx may be not equal to 1.
				// This is because I am too lazy to design carefully.
				// So I just use step instead of dx*step here.
				x += step;

				y += dy * step;
				z += dz * step;
				u += du * step;
				v += dv * step;
				r += dr * step;
				b += db * step;
				g += dg * step;
#endif
			}
			
			// As I say in class Shadee, the u value it holds is the result of the actual u value multiplied by the z value it holds.
			// In other word, the u value it holds is the result of the actual u value divided by the actual z value.
			// So to get the actual u value, this function returns u/z.
			ILL_INLINE int getU() const { return int(u / z); }

			// As I say in class Shadee, the v value it holds is the result of the actual v value multiplied by the z value it holds.
			// In other word, the v value it holds is the result of the actual v value divided by the actual z value.
			// So to get the actual v value, this function returns v/z.
			ILL_INLINE int getV() const { return int(v / z); }
		};


		// Class LerpZ is a helper class that interpolates along the z-axis.
		// ATTENTION:
		// Class LerpZ is completely different from class LerpX or class LerpY.
		// It cannot be considered a Shadee and only provide static method.
		class LerpZ {
		public:
			// This function is used for frustum cutting.
			// It calculates the point SRC whose z equals a given value on the line between two points A and B.
			// It saves the point SRC in 3D space to the parameter 'v_src',
			// and saves the point SRC in screen space to the parameter 'src'.
			// As I say above, the z value of each Shadee is not z coordinates but z depth,
			// so the z value is related to camera orientation.
			// Therefore, parameter 'vz' provides the camera orientation, and 'vz_mod' provides its length.
			// The parameter 'z' provides that given value, that is, src.z is equal to the parameter z.
			// Finally, the parameter va,vb provides the points A,B in 3D space, 
			// and the parameter sa,sb provides them in screen space.
			static void cut(Shadee& src, Vector3D& v_src, const Vector3D & va, const Vector3D & vb,
				const Shadee& sa, const Shadee& sb, const Vector3D& vz, scalar vz_mod, scalar z) 
			{
#ifdef ILL_SSE_IN_API
				scalar ILL_ATTRIBUTE_ALIGNED16(t) = (vb * vz - z * vz_mod) / ((vb - va) * vz);
				scalar ILL_ATTRIBUTE_ALIGNED16(rt) = scalar(1) - t;
				f4 m_t = _mm_load_ps1(&t);
				f4 m_rt = _mm_load_ps1(&rt);
				v_src.m_vec128 = _mm_add_ps(_mm_mul_ps(va.m_vec128, m_t), _mm_mul_ps(vb.m_vec128, m_rt));
				t /= sa.z * z;
				rt /= sb.z * z;
				m_t = _mm_load_ps1(&t);
				m_rt = _mm_load_ps1(&rt);
				src.m_xyuv = _mm_add_ps(_mm_mul_ps(sa.m_xyuv, m_t), _mm_mul_ps(sb.m_xyuv, m_rt));
				src.m_bgrz = _mm_add_ps(_mm_mul_ps(sa.m_bgrz, m_t), _mm_mul_ps(sb.m_bgrz, m_rt));
#else
				scalar t = (vb * vz - z * vz_mod) / ((vb - va) * vz);
				scalar rt = scalar(1) - t;
				v_src.x = va.x * t + vb.x * rt;
				v_src.y = va.y * t + vb.y * rt;
				v_src.z = va.z * t + vb.z * rt;
				src.z = scalar(1) / z;
				t /= sa.z * z;
				rt /= sb.z * z;
				src.r = sa.r * t + sb.r * rt;
				src.g = sa.g * t + sb.g * rt;
				src.b = sa.b * t + sb.b * rt;
				src.u = sa.u * t + sb.u * rt;
				src.v = sa.v * t + sb.v * rt;
#endif
			}
		private:
			LerpZ() {}
			~LerpZ() {}
		};


		// It returns cross product of a and b, which is not guaranteed to be unit length.
		ILL_INLINE Vector3D normalVector(const Vector3D& a, const Vector3D& b) {
			return Vector3D(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
		}

		// The AABB is determined by two vectors mi,mx, representing a box whose size is equal to mx-mi and center is (mi+mx)/2.
		class AxisAlignedBoundingBox {
		public:
			Vector3D mi, mx;
			AxisAlignedBoundingBox(const Vector3D& mi, const Vector3D& mx) :mi(mi), mx(mx) {}
			AxisAlignedBoundingBox() :mi(), mx() {}
			ILL_INLINE Vector3D center() const { return (mi + mx) * scalar(0.5); }
			ILL_INLINE Vector3D size() const { return mx - mi; }

			// It tells if this AABB completely contains 'a'.
			ILL_INLINE bool contain(const AxisAlignedBoundingBox& a) const {
				return ((mi.x <= a.mi.x) &&
					(mi.y <= a.mi.y) &&
					(mi.z <= a.mi.z) &&
					(mx.x >= a.mx.x) &&
					(mx.y >= a.mx.y) &&
					(mx.z >= a.mx.z));
			}

			// It tells if this AABB is different from 'a'.
			ILL_INLINE bool nequal(const AxisAlignedBoundingBox& a) const {
				return ((mi.x != a.mi.x) ||
					(mi.y != a.mi.y) ||
					(mi.z != a.mi.z) ||
					(mx.x != a.mx.x) ||
					(mx.y != a.mx.y) ||
					(mx.z != a.mx.z));
			}

			// It merges this AABB with 'a'.
			ILL_INLINE void merge(const AxisAlignedBoundingBox& a) {
#ifdef ILL_SSE
				__m128 ami(_mm_load_ps(a.mi));
				__m128 amx(_mm_load_ps(a.mx));
				ami = _mm_min_ps(ami, _mm_load_ps(mi));
				amx = _mm_max_ps(amx, _mm_load_ps(mx));
				_mm_store_ps(mi, ami);
				_mm_store_ps(mx, amx);
#else 
				for (int i = 0; i < 3; ++i) {
					if (a.mi[i] < mi[i]) mi[i] = a.mi[i];
					if (a.mx[i] > mx[i]) mx[i] = a.mx[i];
				}
#endif
			}

			// It merges 'a' with 'b' and saves the result to this AABB.
			// In addition, it tells if this AABB is changed after the result is saved.
			ILL_INLINE bool merge(const AxisAlignedBoundingBox& a, const AxisAlignedBoundingBox& b) {
				AxisAlignedBoundingBox old(mi, mx);
#ifdef ILL_SSE
				__m128 ami(_mm_load_ps(a.mi));
				__m128 amx(_mm_load_ps(a.mx));
				ami = _mm_min_ps(ami, _mm_load_ps(b.mi));
				amx = _mm_max_ps(amx, _mm_load_ps(b.mx));
				_mm_store_ps(mi, ami);
				_mm_store_ps(mx, amx);
#else 
				for (int i = 0; i < 3; ++i) {
					mi[i] = (a.mi[i] < b.mi[i]) ? a.mi[i] : b.mi[i];
					mx[i] = (a.mx[i] > b.mx[i]) ? a.mx[i] : b.mx[i];
				}
#endif
				return nequal(old);
			}


			// It expands this AABB along the direction indicated by parameter 'e'.
			ILL_INLINE void expand(const Vector3D& e) {
				if (e.x > 0) mx.x = (mx.x + e[0]);
				else mi.x = (mi.x + e[0]);
				if (e.y > 0) mx.y = (mx.y + e[1]);
				else mi.y = (mi.y + e[1]);
				if (e.z > 0) mx.z = (mx.z + e[2]);
				else mi.z = (mi.z + e[2]);
			}

			// It tells if this AABB intersects with 'a'.
			ILL_INLINE bool intersect(const AxisAlignedBoundingBox& a) const {
#ifdef ILL_SSE_IN_API
				const __m128 rt(_mm_or_ps(_mm_cmplt_ps(_mm_load_ps(mx), _mm_load_ps(a.mi)),
					_mm_cmplt_ps(_mm_load_ps(a.mx), _mm_load_ps(mi))));
				const int* pu((const int*)&rt);
				return !(pu[0] | pu[1] | pu[2]);
#else
				return (a.mi.x <= mx.x) &&
					(a.mx.x >= mi.x) &&
					(a.mi.y <= mx.y) &&
					(a.mx.y >= mi.y) &&
					(a.mi.z <= mx.z) &&
					(a.mx.z >= mi.z);
#endif
			}
		};


		scalar rands(scalar c = 0.0f, scalar d = 0.5f) {
			static std::default_random_engine e;
			std::uniform_real_distribution<scalar> u(c - d, c + d);
			return u(e);
		}

		// uniform unit vector
		ILL_INLINE Vector3D random_unit() {
			scalar cos_theta = rands(0, 1), sin_theta = illSqrt(1 - cos_theta * cos_theta);
			scalar phi = rands(0, PI);
			return Vector3D(illCos(phi) * sin_theta, illSin(phi) * sin_theta, cos_theta);
		}

	}
}


fl::geom::LerpX::LerpX(const LerpY & sa, const LerpY & sb)
#ifndef ILL_SSE_IN_API
	: x(sa.x), y(sa.y), z(sa.z), u(sa.u), v(sa.v), r(sa.r), g(sa.g), b(sa.b), dx(1.0 / (sb.x - x)), dy((sb.y - y) * dx), dz((sb.z - z) * dx),
	du((sb.u - u) * dx), dv((sb.v - v) * dx), dr((sb.r - r) * dx), dg((sb.g - g) * dx), db((sb.b - b) * dx)
#endif
{
#ifdef ILL_SSE_IN_API
	m_xyuv = sa.m_xyuv;
	m_bgrz = sa.m_bgrz;
	m_dxyuv = _mm_sub_ps(sb.m_xyuv, sa.m_xyuv);
	m_drgbz = _mm_sub_ps(sb.m_bgrz, sa.m_bgrz);
	f4 mask = _mm_shuffle_ps(m_dxyuv, m_dxyuv, 0x0); //_MM_SHUFFLE(0, 0, 0, 0)
	m_dxyuv = _mm_div_ps(m_dxyuv, mask);
	m_drgbz = _mm_div_ps(m_drgbz, mask);
#endif
}

fl::geom::LerpY::LerpY(const Shadee & sa, const Shadee & sb)
#ifndef ILL_SSE_IN_API
	:x(sa.x), y(sa.y), z(sa.z), u(sa.u), v(sa.v), r(sa.r), g(sa.g), b(sa.b), dy(1.0 / (sb.y - y)),
	dx((sb.x - x) / (sb.y - y)), dz((sb.z - z) * dy), du((sb.u - u) * dy), dv((sb.v - v) * dy),
	dr((sb.r - r) * dy), dg((sb.g - g) * dy), db((sb.b - b) * dy)
#endif
{
#ifdef ILL_SSE_IN_API
	m_xyuv = sa.m_xyuv;
	m_bgrz = sa.m_bgrz;
	m_dxyuv = _mm_sub_ps(sb.m_xyuv, sa.m_xyuv);
	m_drgbz = _mm_sub_ps(sb.m_bgrz, sa.m_bgrz);
	f4 mask = _mm_shuffle_ps(m_dxyuv, m_dxyuv, 0x55); //_MM_SHUFFLE(1, 1, 1, 1)
	m_dxyuv = _mm_div_ps(m_dxyuv, mask);
	m_drgbz = _mm_div_ps(m_drgbz, mask);
#endif
}

fl::geom::LerpY::LerpY(const Shadee & sa, const Shadee & sb, const scalar& step)
#ifndef ILL_SSE_IN_API
	: x(sa.x), y(sa.y), z(sa.z), u(sa.u), v(sa.v), r(sa.r), g(sa.g), b(sa.b), dy(step / (sb.y - y)),
	dx((sb.x - x) * (step / (sb.y - y))), dz((sb.z - z) * dy), du((sb.u - u) * dy), dv((sb.v - v) * dy),
	dr((sb.r - r) * dy), dg((sb.g - g) * dy), db((sb.b - b) * dy)
#endif
{
#ifdef ILL_SSE_IN_API
	m_xyuv = sa.m_xyuv;
	m_bgrz = sa.m_bgrz;
	m_dxyuv = _mm_sub_ps(sb.m_xyuv, sa.m_xyuv);
	m_drgbz = _mm_sub_ps(sb.m_bgrz, sa.m_bgrz);
	f4 mask = _mm_div_ps(_mm_shuffle_ps(m_dxyuv, m_dxyuv, 0x55), _mm_load_ps1(&step)); //_MM_SHUFFLE(1, 1, 1, 1)
	m_dxyuv = _mm_div_ps(m_dxyuv, mask);
	m_drgbz = _mm_div_ps(m_drgbz, mask);
#endif
}

