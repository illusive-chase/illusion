#pragma once
#include "SGeomMath.h"
#include "STexture.h"

namespace fl {
	namespace geom {

		ILL_ATTRIBUTE_ALIGNED16(struct) MapTrait {
			float z_depth, r, g, b;
			void* object;
			MapTrait() :z_depth(0), r(0), g(0), b(0), object(nullptr) {}

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

			Shader(DWORD* write_src, MapTrait* map_trait_src, int width, void* obj, const Texture& t, const int offset = 0);
			void moveTo(int x, int y, int channel);
			void move(int i);
			void move();
			void shade(const LerpX& shadee, DWORD mask = 0x00FFFFFFU);
			void shade_follow(float z, int id);
		};
	}
}