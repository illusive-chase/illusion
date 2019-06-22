/// @file SGeomMath.h
/// @brief 3D数学库，包括矩阵类、RGB混合方法、定点数类、向量类、着色点类、线性插值类
/// @date 2019/6/22

#pragma once

#include "Scalar.h"
#include "SAlignedAllocator.h"
#include "SColor.h"

namespace fl {
	namespace geom {


		class Rad {
		public:
			scalar s, c;
			Rad(scalar rad) :s(illSin(rad)), c(illCos(rad)) {}
			Rad(scalar s, scalar c) :s(s), c(c) {}
			Rad operator -() const { return Rad(-s, c); }
		};

		/// @brief 矩阵类（未完成）
		///
		class Matrix3D {
		public:
			scalar value[4][4];            ///< 矩阵各项值
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
		};

		/// @brief 将UV坐标映射成一个32位整型
		/// @param[in] u U坐标，0~65535的整数
		/// @param[in] v V坐标，0~65535的整数
		/// @return DWORD
		ILL_INLINE DWORD UV(DWORD u, DWORD v) {
			return u << 16 | v;
		}

		/// @brief 根据权重调整R/G/B，返回调整后的值
		/// @param[in] a R/G/B值，0~255的整数
		/// @param[in] w 权重，非负实数
		/// @return BYTE
		ILL_INLINE BYTE MIX(BYTE a, scalar w) {
			int c = int(a * w);
			return c > 0xFF ? 0xFF : BYTE(c);
		}

		/// @brief 根据权重混合两个RGB，混合后的值赋值给第一个参数
		/// @param[in] c RGB值，32位整型
		/// @param[in] w_e 参数e表示的颜色的权重，0~1的实数
		/// @param[in] e RGB值，32位整型
		/// @note 第一个参数的最高4位不会改变
		/// @return void
		ILL_INLINE void MIX32(DWORD& c, scalar w_e, DWORD e) {
			scalar w_c = 1.0f - w_e;
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

		/// @brief 顶点数（未完成）
		///
		class fixed {
		public:
			const static lint FIXED_MAX = (1i64 << 55) - 1i64;
			const static lint FIXED_MIN = (-1i64 << 55);

			explicit fixed(int x) :value(x << 8) {}
			explicit fixed(scalar x) :value(int(x) << 8) {}
			fixed() :value(0) {}
			operator int() { return int(value >> 8); }
			operator scalar() { return value / scalar(65536); }

			//fixed

			ILL_INLINE fixed operator *(const fixed& t) { return fixed((t.value * value) >> 8); }
			ILL_INLINE fixed operator +(const fixed& t) { return fixed(t.value + value); }
			ILL_INLINE fixed operator /(const fixed& t) { return fixed((value << 8) / t.value); }
			ILL_INLINE fixed operator -(const fixed& t) { return fixed(value - t.value); }
			ILL_INLINE void operator ++() { value += 1 << 8; }
			ILL_INLINE void operator --() { value -= 1 << 8; }
			ILL_INLINE void operator +=(const fixed& t) { value += t.value; }
			ILL_INLINE void operator -=(const fixed& t) { value -= t.value; }
			ILL_INLINE void operator *=(const fixed& t) { value *= t.value; value >>= 8; }
			ILL_INLINE void operator /=(const fixed& t) { value <<= 8; value /= t.value; }
			ILL_INLINE bool operator >(const fixed& t)const { return value > t.value; }
			ILL_INLINE bool operator >=(const fixed& t)const { return value >= t.value; }
			ILL_INLINE bool operator <(const fixed& t)const { return value < t.value; }
			ILL_INLINE bool operator <=(const fixed& t)const { return value <= t.value; }
			ILL_INLINE bool operator ==(const fixed& t)const { return value == t.value; }
			ILL_INLINE const fixed& operator =(const fixed& t) { value = t.value; return *this; }
			ILL_INLINE fixed operator -() { return fixed(-value); }

			//int

			ILL_INLINE fixed operator *(const int& t) { return fixed(lint(t) * value); }
			ILL_INLINE fixed operator +(const int& t) { return fixed((lint(t << 8)) + value); }
			ILL_INLINE fixed operator /(const int& t) { return fixed(value / lint(t)); }
			ILL_INLINE fixed operator -(const int& t) { return fixed(value - lint(t << 8)); }
			ILL_INLINE void operator +=(const int& t) { value += t << 8; }
			ILL_INLINE void operator -=(const int& t) { value -= t << 8; }
			ILL_INLINE void operator *=(const int& t) { value *= t; }
			ILL_INLINE void operator /=(const int& t) { value /= t; }
			ILL_INLINE bool operator >(const int& t)const { return value > (t << 8); }
			ILL_INLINE bool operator >=(const int& t)const { return value >= (t << 8); }
			ILL_INLINE bool operator <(const int& t)const { return value < (t << 8); }
			ILL_INLINE bool operator <=(const int& t)const { return value <= (t << 8); }
			ILL_INLINE bool operator ==(const int& t)const { return value == (t << 8); }
			ILL_INLINE const fixed& operator =(const int& t) { value = t << 8; return *this; }

		private:
			lint value;
			explicit fixed(lint x) :value(x) {}
		};




		/// @brief 向量类
		///
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

			Vector3D(scalar x, scalar y, scalar z) :x(x), y(y), z(z), w(0) {}

			ILL_INLINE Vector3D operator -() const { return Vector3D(-x, -y, -z); }

			ILL_INLINE Vector3D& operator +=(const Vector3D& tar) {
#ifdef ILL_SSE_IN_API
				m_vec128 = _mm_add_ps(m_vec128, tar.m_vec128);
#else
				x += tar.x; y += tar.y; z += tar.z;
#endif
				return *this;
			}

			ILL_INLINE Vector3D operator +(const Vector3D& tar) const { 
#ifdef ILL_SSE_IN_API
				return Vector3D(_mm_add_ps(m_vec128, tar.m_vec128));
#else
				return Vector3D(x + tar.x, y + tar.y, z + tar.z);
#endif
			}

			ILL_INLINE Vector3D& operator -=(const Vector3D& tar) {
#ifdef ILL_SSE_IN_API
				m_vec128 = _mm_add_ps(m_vec128, tar.m_vec128);
#else
				x -= tar.x; y -= tar.y; z -= tar.z; 
#endif
				return *this;
			}

			ILL_INLINE Vector3D operator -(const Vector3D& tar)  const { 
#ifdef ILL_SSE_IN_API
				return Vector3D(_mm_sub_ps(m_vec128, tar.m_vec128));
#else
				return Vector3D(x - tar.x, y - tar.y, z - tar.z);
#endif
			}

			ILL_INLINE Vector3D& operator *=(const scalar& k) {
#ifdef ILL_SSE_IN_API
				__m128 vs = _mm_load_ss(&k);  //	(S 0 0 0)
				vs = ill_pshufd_ps(vs, 0x80);  //	(S S S 0.0)  0x80 = _MM_SHUFFLE(2, 0, 0, 0)
				m_vec128 = _mm_mul_ps(m_vec128, vs);
#else
				x *= k; y *= k; z *= k;
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
				return x * tar.x + y * tar.y + z * tar.z;
#endif
			}

			ILL_INLINE Vector3D operator *(const scalar& k) const {
#ifdef ILL_SSE_IN_API
				__m128 vs = _mm_load_ss(&k);  //	(S 0 0 0)
				vs = ill_pshufd_ps(vs, 0x80);  //	(S S S 0.0)  0x80 = _MM_SHUFFLE(2, 0, 0, 0)
				return Vector3D(_mm_mul_ps(m_vec128, vs));
#else
				return Vector3D(x * k, y * k, z * k);
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

				y = ill_splat_ps(y, 0x80);
				m_vec128 = _mm_mul_ps(m_vec128, y);
#else
				scalar k = mod();
				x /= k;	y /= k;	z /= k;
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

		};

		ILL_ATTRIBUTE_ALIGNED16(class) Shadee {
		public:
			ILL_DECLARE_ALIGNED_ALLOCATOR

#ifdef ILL_SSE
			union {
				struct { scalar x, y, u, v, r, g, b, z; };
				struct { f4 m_xyuv, m_rgbz; };
				struct { scalar floats[8]; };
			};
#else
			union {
				struct { scalar x, y, u, v, r, g, b, z; };
				struct { scalar floats[8]; };
			};
#endif
			Shadee() {}
			Shadee(Shadee* va, Shadee* vb, scalar y0) {
				scalar ILL_ATTRIBUTE_ALIGNED16(t) = (y0 - va->y) / (vb->y - va->y);
				scalar ILL_ATTRIBUTE_ALIGNED16(rt) = scalar(1) - t;
#ifdef ILL_SSE_IN_API
				{
					__m128 m_t = _mm_load1_ps(&t);
					__m128 m_rt = _mm_load1_ps(&rt);
					m_xyuv = _mm_add_ps(_mm_mul_ps(va->m_xyuv, m_rt), _mm_mul_ps(vb->m_xyuv, m_t));
					m_rgbz = _mm_add_ps(_mm_mul_ps(va->m_rgbz, m_rt), _mm_mul_ps(vb->m_rgbz, m_t));
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
			ILL_INLINE void set(scalar _x, scalar _y, const Vector3D& color, scalar _z) {
				x = _x;
				y = _y;
#ifdef ILL_SSE
				m_rgbz = color.m_vec128;
#else
				r = color.x;
				g = color.y;
				b = color.z;
#endif
				z = _z;
			}
			static bool clockwise(const Shadee& a, const Shadee& b, const Shadee& c) {
				return (c.x - a.x)*b.y + (b.x - c.x)*a.y + (a.x - b.x)*c.y < scalar(0);
			}
		};

		ILL_ATTRIBUTE_ALIGNED16(class) LerpY {
		public:
			ILL_DECLARE_ALIGNED_ALLOCATOR

#ifdef ILL_SSE
			union {
				struct { scalar x, y, u, v, r, g, b, z, dx, dy, du, dv, dr, dg, db, dz; };
				struct { f4 m_xyuv, m_rgbz, m_dxyuv, m_drgbz; };
				struct { scalar floats[16]; };
			};
#else
			union {
				struct { scalar x, y, u, v, r, g, b, z, dx, dy, du, dv, dr, dg, db, dz; };
				struct { scalar floats[16]; };
			};
#endif
			LerpY(const Shadee& sa, const Shadee& sb, const scalar& step);
			LerpY(const Shadee& sa, const Shadee& sb);

			ILL_INLINE void start(int ay, scalar sample_y){ move(ay - y + sample_y); }

			ILL_INLINE void move() {
#ifdef ILL_SSE_IN_API
				m_rgbz = _mm_add_ps(m_rgbz, m_drgbz);
				m_xyuv = _mm_add_ps(m_xyuv, m_dxyuv);
#else
				x += dx;
				y += scalar(1);
				z += dz;
				u += du;
				v += dv;
				r += dr;
				b += db;
				g += dg;
#endif
			}

			ILL_INLINE void move(const scalar& step) {
#ifdef ILL_SSE_IN_API
				f4 m_step = _mm_load_ps1(&step);
				m_rgbz = _mm_add_ps(m_rgbz, _mm_mul_ps(m_drgbz, m_step));
				m_xyuv = _mm_add_ps(m_xyuv, _mm_mul_ps(m_dxyuv, m_step));
#else
				x += dx * step;
				y += step;
				z += dz * step;
				u += du * step;
				v += dv * step;
				r += dr * step;
				b += db * step;
				g += dg * step;
#endif
			}


			ILL_INLINE void reset(const Shadee& sa) {
#ifdef ILL_SSE
				m_xyuv = sa.m_xyuv;
				m_rgbz = sa.m_rgbz;
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

		ILL_ATTRIBUTE_ALIGNED16(class) LerpX {
		public:
			ILL_DECLARE_ALIGNED_ALLOCATOR

#ifdef ILL_SSE
			union {
				struct { scalar x, y, u, v, r, g, b, z, dx, dy, du, dv, dr, dg, db, dz; };
				struct { f4 m_xyuv, m_rgbz, m_dxyuv, m_drgbz; };
				struct { scalar floats[16]; };
			};
#else
			union {
				struct { scalar x, y, u, v, r, g, b, z, dx, dy, du, dv, dr, dg, db, dz; };
				struct { scalar floats[16]; };
			};
#endif
			LerpX(const LerpY& sa, const LerpY& sb);
			ILL_INLINE void start(int ax, scalar sample_x) { move(ax - x + sample_x); }

			ILL_INLINE void move() {
#ifdef ILL_SSE_IN_API
				m_rgbz = _mm_add_ps(m_rgbz, m_drgbz);
				m_xyuv = _mm_add_ps(m_xyuv, m_dxyuv);
#else
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

			ILL_INLINE void move(const scalar& step) {
#ifdef ILL_SSE_IN_API
				f4 m_step = _mm_load_ps1(&step);
				m_rgbz = _mm_add_ps(m_rgbz, _mm_mul_ps(m_drgbz, m_step));
				m_xyuv = _mm_add_ps(m_xyuv, _mm_mul_ps(m_dxyuv, m_step));
#else
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

			ILL_INLINE int getU() const { return int(u / z); }
			ILL_INLINE int getV() const { return int(v / z); }
		};

		class LerpZ {
		public:
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
				src.m_rgbz = _mm_add_ps(_mm_mul_ps(sa.m_rgbz, m_t), _mm_mul_ps(sb.m_rgbz, m_rt));
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


		//02*01
		ILL_INLINE Vector3D normalVector(const Vector3D& a, const Vector3D& b) {
			return Vector3D(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
		}

	}
}