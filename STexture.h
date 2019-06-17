#pragma once
#include "SConfig.h"

namespace fl {
	namespace geom {
		class Texture {
		public:
			DWORD* src;
			int width, height;
			float Ka, Kd, Ks;
			Texture(DWORD* src, int width, int height, float Ka = 0.8f, float Kd = 0.8f, float Ks = 0.1f)
				:src(src), Ka(Ka), Kd(Kd), Ks(Ks), width(width), height(height) {
			}
			Texture(DWORD& pure_color, float Ka = 0.8f, float Kd = 0.8f, float Ks = 0.1f) :src(&pure_color), Ka(Ka), Kd(Kd), Ks(Ks), width(1), height(1) {}
		};
	}
}