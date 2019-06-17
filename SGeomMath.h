/// @file SGeomMath.h
/// @brief 3D��ѧ�⣬���������ࡢRGB��Ϸ������������ࡢ�����ࡢ��ɫ���ࡢ���Բ�ֵ��
/// @date 2019/3/31

#pragma once

#include "SConfig.h"
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

		/// @brief �����ࣨδ��ɣ�
		///
		class Matrix3D {
		public:
			float value[4][4];            ///< �������ֵ
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

		/// @brief ��UV����ӳ���һ��32λ����
		/// @param[in] u U���꣬0~65535������
		/// @param[in] v V���꣬0~65535������
		/// @return DWORD
		constexpr DWORD UV(DWORD u, DWORD v) {
			return u << 16 | v;
		}

		/// @brief ����Ȩ�ص���R/G/B�����ص������ֵ
		/// @param[in] a R/G/Bֵ��0~255������
		/// @param[in] w Ȩ�أ��Ǹ�ʵ��
		/// @return BYTE
		constexpr BYTE MIX(BYTE a, float w) {
			int c = int(a * w);
			return c > 0xFF ? 0xFF : BYTE(c);
		}

		/// @brief ����Ȩ�ػ������RGB����Ϻ��ֵ��ֵ����һ������
		/// @param[in] c RGBֵ��32λ����
		/// @param[in] w_e ����e��ʾ����ɫ��Ȩ�أ�0~1��ʵ��
		/// @param[in] e RGBֵ��32λ����
		/// @note ��һ�����������4λ����ı�
		/// @return void
		inline void MIX32(DWORD& c, float w_e, DWORD e) {
			float w_c = 1.0f - w_e;
#ifdef SSE
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

		/// @brief ��������δ��ɣ�
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

			inline fixed operator *(const fixed& t) { return fixed((t.value * value) >> 8); }
			inline fixed operator +(const fixed& t) { return fixed(t.value + value); }
			inline fixed operator /(const fixed& t) { return fixed((value << 8) / t.value); }
			inline fixed operator -(const fixed& t) { return fixed(value - t.value); }
			inline void operator ++() { value += 1 << 8; }
			inline void operator --() { value -= 1 << 8; }
			inline void operator +=(const fixed& t) { value += t.value; }
			inline void operator -=(const fixed& t) { value -= t.value; }
			inline void operator *=(const fixed& t) { value *= t.value; value >>= 8; }
			inline void operator /=(const fixed& t) { value <<= 8; value /= t.value; }
			inline bool operator >(const fixed& t)const { return value > t.value; }
			inline bool operator >=(const fixed& t)const { return value >= t.value; }
			inline bool operator <(const fixed& t)const { return value < t.value; }
			inline bool operator <=(const fixed& t)const { return value <= t.value; }
			inline bool operator ==(const fixed& t)const { return value == t.value; }
			inline const fixed& operator =(const fixed& t) { value = t.value; return *this; }
			inline fixed operator -() { return fixed(-value); }

			//int

			inline fixed operator *(const int& t) { return fixed(lint(t) * value); }
			inline fixed operator +(const int& t) { return fixed((lint(t << 8)) + value); }
			inline fixed operator /(const int& t) { return fixed(value / lint(t)); }
			inline fixed operator -(const int& t) { return fixed(value - lint(t << 8)); }
			inline void operator +=(const int& t) { value += t << 8; }
			inline void operator -=(const int& t) { value -= t << 8; }
			inline void operator *=(const int& t) { value *= t; }
			inline void operator /=(const int& t) { value /= t; }
			inline bool operator >(const int& t)const { return value > (t << 8); }
			inline bool operator >=(const int& t)const { return value >= (t << 8); }
			inline bool operator <(const int& t)const { return value < (t << 8); }
			inline bool operator <=(const int& t)const { return value <= (t << 8); }
			inline bool operator ==(const int& t)const { return value == (t << 8); }
			inline const fixed& operator =(const int& t) { value = t << 8; return *this; }

		private:
			lint value;
			explicit fixed(lint x) :value(x) {}
		};

		/// @brief ������
		///
		class Vector3D {
		public:
			float x, y, z;
			uint w;

			Vector3D() :x(0), y(0), z(0), w(0U) {}

			Vector3D(float x, float y, float z) :x(x), y(y), z(z), w(0U) {}

			inline void operator +=(const Vector3D& tar) { x += tar.x; y += tar.y; z += tar.z; }

			inline Vector3D operator +(const Vector3D& tar) const { return Vector3D(x + tar.x, y + tar.y, z + tar.z); }

			inline void operator -=(const Vector3D& tar) { x -= tar.x; y -= tar.y; z -= tar.z; }

			inline Vector3D operator -(const Vector3D& tar)  const { return Vector3D(x - tar.x, y - tar.y, z - tar.z); }

			inline void operator *=(float k) { x *= k; y *= k; z *= k; }

			inline void operator /=(float k) { x /= k; y /= k;	z /= k; }

			inline float mod() const { return sqrt(x * x + y * y + z * z); }

			inline float mod2() const { return (x * x + y * y + z * z); }

			inline void normalize() { float k = mod();	x /= k;	y /= k;	z /= k; }

			inline float operator *(const Vector3D& tar) const { return x * tar.x + y * tar.y + z * tar.z; }

			inline Vector3D operator *(float k) const { return Vector3D(x * k, y * k, z * k); }

			inline bool operator ==(const Vector3D& cp) const {
				return abs(x - cp.x) < 1e-5 && abs(y - cp.y) < 1e-5 && abs(z - cp.z) < 1e-5;
			}

			inline void rotateX(const Rad& rad) {
				float temp = rad.c * y + rad.s * z;
				z = rad.c * z - rad.s * y;
				y = temp;
			}

			inline void rotateY(const Rad& rad) {
				float temp = rad.c * x - rad.s * z;
				z = rad.c * z + rad.s * x;
				x = temp;
			}

			inline void rotateZ(const Rad& rad) {
				float temp = rad.c * x + rad.s * y;
				y = rad.c * y - rad.s * x;
				x = temp;
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
			inline void set(const float& _x, const float& _y, const float& _z, const float& _r, const float& _g, const float& _b) {
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
			inline int getU() const { return int(u / z); }
			inline int getV() const { return int(v / z); }
		};

		class LerpZ {
		public:
			static inline void cut(Shadee& src, Vector3D& v_src, const Vector3D & va, const Vector3D & vb,
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

		inline bool triangleClockwise(Shadee *p) {
			return ((p[1].x - p[0].x) * (p[2].y - p[0].y) - (p[1].y - p[0].y) * (p[2].x - p[0].x) >= 0);
		}

		//02*01
		inline Vector3D normalVector(const Vector3D& a, const Vector3D& b) {
			return Vector3D(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
		}

	}
}