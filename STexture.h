#pragma once
#include "Scalar.h"

namespace fl {
	namespace geom {
		class Texture {
		public:
			DWORD* src;
			int width, height;
			scalar Ka, Kd, Ks;
			Texture(DWORD* src, int width, int height, scalar Ka = 0.8f, scalar Kd = 0.8f, scalar Ks = 0.1f)
				:src(src), Ka(Ka), Kd(Kd), Ks(Ks), width(width), height(height) {
			}
			Texture(DWORD& pure_color, scalar Ka = 0.8f, scalar Kd = 0.8f, scalar Ks = 0.1f) :src(&pure_color), Ka(Ka), Kd(Kd), Ks(Ks), width(1), height(1) {}
		};
	}
}