#pragma once
#include "SGeomMath.h"
#include "STexture.h"

namespace fl {
	namespace geom {

		ILL_ATTRIBUTE_ALIGNED16(struct) MapTrait {
#ifdef ILL_SSE
			union {
				struct { scalar r, g, b, z_depth; };
				struct { f4 m_rgbz; };
			};
#else
			scalar z_depth, r, g, b;
#endif
#ifdef ILL_DEBUG
			void* object;
			MapTrait() :z_depth(0), r(0), g(0), b(0), object(nullptr) {}
#endif

			MapTrait() :z_depth(0), r(0), g(0), b(0) {}

			ILL_DECLARE_ALIGNED_ALLOCATOR
		};

		class Shader {
		public:
			const BYTE* const src;
			void* obj;

			const int src_wid;
			const int src_size;
			const int offset;
			const int step;
			const int width;

			DWORD* const write_src;
			MapTrait* const map_trait_src;

			DWORD* write;
			MapTrait* map_trait;

			Shader(DWORD* write_src, MapTrait* map_trait_src, int width, void* obj, const Texture& t, const int offset = 0)
				:src(reinterpret_cast<const BYTE*>(t.src)), src_wid(t.width), src_size(t.width * t.height),
				offset(offset), step(1 << offset), write_src(write_src),
				map_trait_src(map_trait_src), width(width), obj(obj) 
			{
				write = write_src;
				map_trait = map_trait_src;
			}

			ILL_INLINE void moveTo(int x, int y, int channel) {
				int pos = ((x + y * width) << offset) | channel;
				write = write_src + pos;
				map_trait = map_trait_src + pos;
			}

			ILL_INLINE void move(int i) {
				write += i << offset;
				map_trait += i << offset;
			}

			ILL_INLINE void move() {
				write += step;
				map_trait += step;
			}

			ILL_INLINE void shade(const LerpX& shadee, DWORD mask = 0x00FFFFFFU) {
				if (map_trait->z_depth < shadee.z) {
					int index = shadee.getU() + src_wid * shadee.getV();
					//if (index < 0 || index >= src_size) return;
					*write = reinterpret_cast<const DWORD*>(src)[index];
#ifdef ILL_SSE
					map_trait->m_rgbz = shadee.m_rgbz;
#else
					map_trait->r = shadee.r;
					map_trait->g = shadee.g;
					map_trait->b = shadee.b;
					map_trait->z_depth = shadee.z;
#endif
#ifdef ILL_DEBUG
					map_trait->object = obj;
#endif
				}
			}

			ILL_INLINE void shade_follow(scalar z, int id) {
				if (map_trait->z_depth < z) {
					*write = write[-id];
					*map_trait = map_trait[-id];
					map_trait->z_depth = z;
				}
			}


		};
	}
}