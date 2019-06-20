/// @file SGeomMath.h
/// @brief 3D数学库，包括矩阵类、RGB混合方法、定点数类、向量类、着色点类、线性插值类
/// @date 2019/3/31

#pragma once

#include "Scalar.h"
#include "SAlignedAllocator.h"
#include "SColor.h"

namespace fl {
	namespace geom {


		class Rad {
		public:
			float s, c;
			Rad(float rad) :s(sin(rad)), c(cos(rad)) {}
			Rad(float s, float c) :s(s), c(c) {}
			Rad operator -() const { return Rad(-s, c); }
		};

		/// @brief 矩阵类（未完成）
		///
		class Matrix3D {
		public:
			float value[4][4];            ///< 矩阵各项值
			Matrix3D() {
				memset(value, 0, sizeof value);
			}
			Matrix3D(const std::initializer_list<float>& list) {
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
		constexpr DWORD UV(DWORD u, DWORD v) {
			return u << 16 | v;
		}

		/// @brief 根据权重调整R/G/B，返回调整后的值
		/// @param[in] a R/G/B值，0~255的整数
		/// @param[in] w 权重，非负实数
		/// @return BYTE
		constexpr BYTE MIX(BYTE a, float w) {
			int c = int(a * w);
			return c > 0xFF ? 0xFF : BYTE(c);
		}

		/// @brief 根据权重混合两个RGB，混合后的值赋值给第一个参数
		/// @param[in] c RGB值，32位整型
		/// @param[in] w_e 参数e表示的颜色的权重，0~1的实数
		/// @param[in] e RGB值，32位整型
		/// @note 第一个参数的最高4位不会改变
		/// @return void
		ILL_INLINE void MIX32(DWORD& c, float w_e, DWORD e) {
			float w_c = 1.0f - w_e;
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
			explicit fixed(float x) :value(int(x) << 8) {}
			fixed() :value(0) {}
			operator int() { return int(value >> 8); }
			operator float() { return value / 65536.0; }

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
				float m_floats[4];
				struct {
					float x, y, z, w;
				};
			};

			Vector3D(f4 m_vec128) :m_vec128(m_vec128) {}

#else
			union {
				float m_floats[4];
				struct {
					float x, y, z, w;
				};
			};
#endif
			ILL_DECLARE_ALIGNED_ALLOCATOR

			Vector3D() :x(0), y(0), z(0), w(0) {}

			Vector3D(float x, float y, float z) :x(x), y(y), z(z), w(0) {}

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

			ILL_INLINE Vector3D& operator *=(const float& k) {
#ifdef ILL_SSE_IN_API
				__m128 vs = _mm_load_ss(&k);  //	(S 0 0 0)
				vs = ill_pshufd_ps(vs, 0x80);  //	(S S S 0.0)
				m_vec128 = _mm_mul_ps(m_vec128, vs);
#else
				x *= k; y *= k; z *= k;
#endif
				return *this;
			}

			ILL_INLINE Vector3D& operator /=(float k) { return *this *= (scalar(1) / k); }

			ILL_INLINE float operator *(const Vector3D& tar) const { 
#ifdef ILL_SSE_IN_API
				__m128 vd = _mm_mul_ps(m_vec128, tar.m_vec128);
				__m128 z = _mm_movehl_ps(vd, vd);
				__m128 y = _mm_shuffle_ps(vd, vd, 0x55);
				vd = _mm_add_ss(vd, y);
				vd = _mm_add_ss(vd, z);
				return _mm_cvtss_f32(vd);
#else
				return x * tar.x + y * tar.y + z * tar.z;
#endif
			}

			ILL_INLINE Vector3D operator *(float k) const { return Vector3D(x * k, y * k, z * k); }

			ILL_INLINE float mod2() const { return (*this) * (*this); }

			ILL_INLINE float mod() const { return illSqrt(mod2()); }

			ILL_INLINE Vector3D& normalize() {
#ifdef ILL_SSE_IN_API
				__m128 vd = _mm_mul_ps(m_vec128, m_vec128);
				__m128 z = _mm_movehl_ps(vd, vd);
				__m128 y = _mm_shuffle_ps(vd, vd, 0x55);
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
				float k = mod();
				x /= k;	y /= k;	z /= k;
#endif
				return *this;
			}

			ILL_INLINE bool operator ==(const Vector3D& cp) const {
				return Eq(x, cp.x) && Eq(y, cp.y) && Eq(z, cp.z);
			}

			ILL_INLINE Vector3D& rotateX(const Rad& rad) {
				float temp = rad.c * y + rad.s * z;
				z = rad.c * z - rad.s * y;
				y = temp;
				return *this;
			}

			ILL_INLINE Vector3D& rotateY(const Rad& rad) {
				float temp = rad.c * x - rad.s * z;
				z = rad.c * z + rad.s * x;
				x = temp;
				return *this;
			}

			ILL_INLINE Vector3D& rotateZ(const Rad& rad) {
				float temp = rad.c * x + rad.s * y;
				y = rad.c * y - rad.s * x;
				x = temp;
				return *this;
			}

		};

		class Shadee {
		public:
			float x, y, z;
			float u, v;
			float r, g, b;
			Shadee() {}
			Shadee(float x, float y, float z, float r, float g, float b, float u = 0.0, float v = 0.0) :
				x(x), y(y), z(z), u(u), v(v), r(r), g(g), b(b) {
			}
			Shadee(Shadee* va, Shadee* vb, float y) :y(y) {
				float t = (y - va->y) / (vb->y - va->y);
				float rt = 1.0 - t;
				x = va->x * rt + vb->x * t;
				rt /= va->z; t /= vb->z;
				z = 1.0 / (rt + t); rt *= z; t *= z;
				r = va->r * rt + vb->r * t;
				g = va->g * rt + vb->g * t;
				b = va->b * rt + vb->b * t;
				u = va->u * rt + vb->u * t;
				v = va->v * rt + vb->v * t;
			}
			ILL_INLINE void set(const float& _x, const float& _y, const float& _z, const float& _r, const float& _g, const float& _b) {
				x = _x;
				y = _y;
				z = _z;
				r = _r;
				g = _g;
				b = _b;
			}
			static bool clockwise(const Shadee& a, const Shadee& b, const Shadee& c) {
				return (c.x - a.x)*b.y + (b.x - c.x)*a.y + (a.x - b.x)*c.y < 0.0;
			}
		};

		class LerpY {
		public:
			float x, z, u, v, r, g, b;
			const float y, dy, dx, dz, du, dv, dr, dg, db;
			LerpY(const Shadee& sa, const Shadee& sb, float step);
			LerpY(const Shadee& sa, const Shadee& sb);
			void start(int ay, float sample_y);
			void move();
			void move(float step);
			void reset(const Shadee& sa);
		};

		class LerpX {
		public:
			float z, u, v, r, g, b;
			const float x, dx, dz, du, dv, dr, dg, db;
			LerpX(const LerpY& sa, const LerpY& sb);
			void start(int ax, float sample_x);
			void move();
			void move(float step);
			ILL_INLINE int getU() const { return int(u / z); }
			ILL_INLINE int getV() const { return int(v / z); }
		};

		class LerpZ {
		public:
			static ILL_INLINE void cut(Shadee& src, Vector3D& v_src, const Vector3D & va, const Vector3D & vb,
				const Shadee& sa, const Shadee& sb, const Vector3D& vz, float vz_mod, float z) {
				float t = (vb * vz - z * vz_mod) / ((vb - va) * vz);
				float rt = 1.0 - t;
				v_src.x = va.x * t + vb.x * rt;
				v_src.y = va.y * t + vb.y * rt;
				v_src.z = va.z * t + vb.z * rt;
				src.z = z;
				src.r = sa.r * t + sb.r * rt;
				src.g = sa.g * t + sb.g * rt;
				src.b = sa.b * t + sb.b * rt;
				src.u = sa.u * t + sb.u * rt;
				src.v = sa.v * t + sb.v * rt;
			}
		private:
			LerpZ() {}
			~LerpZ() {}
		};

		ILL_INLINE bool triangleClockwise(Shadee *p) {
			return ((p[1].x - p[0].x) * (p[2].y - p[0].y) - (p[1].y - p[0].y) * (p[2].x - p[0].x) >= 0);
		}

		//02*01
		ILL_INLINE Vector3D normalVector(const Vector3D& a, const Vector3D& b) {
			return Vector3D(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
		}

	}
}