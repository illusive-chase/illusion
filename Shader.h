#pragma once
#include "SGeomMath.h"
#include "STexture.h"

namespace fl {
	namespace geom {

		struct MapTrait {
			float z_depth;
			void* object;
			MapTrait() :z_depth(0), object(nullptr) {}
		};

		class Shader {
		public:
			const BYTE* const src;
			void* obj;

			const int src_wid;
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