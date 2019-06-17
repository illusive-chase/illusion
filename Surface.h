#pragma once
#include "STexture.h"

namespace fl {
	namespace geom {
		class Surface3D {
		public:
			int link_index[3];

			Texture texture;
			int uv[3];

			Surface3D(int pa, int pb, int pc, const  Texture& texture, int uv0, int uv1, int uv2) :texture(texture) {
				link_index[0] = pa;
				link_index[1] = pb;
				link_index[2] = pc;
				uv[0] = uv0;
				uv[1] = uv1;
				uv[2] = uv2;
			}
		};
	}
}