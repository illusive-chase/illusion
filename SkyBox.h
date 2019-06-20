#pragma once
#include "SObject3D.h"

namespace fl {
	namespace geom {

		class SkyBox :public SObject3D {
		public:
			DWORD* top_src;
			const int size;
			inline void framing() {}

			SkyBox(const Texture& tex, int size);

			~SkyBox() { if (top_src) delete[] top_src; }
		};
	}
}

