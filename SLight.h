#pragma once

#include "SPointer.h"
#include "SGeomMath.h"

namespace fl {
	namespace geom {
		ILL_ATTRIBUTE_ALIGNED16(class) Light3D :public AutoPtr {
		public:
			Vector3D intensity;
			int type;
			Light3D(const Vector3D& intensity) :AutoPtr(), intensity(intensity), type(0) {}
			virtual ~Light3D() {}

			ILL_DECLARE_ALIGNED_ALLOCATOR
		};

		ILL_ATTRIBUTE_ALIGNED16(class) PointLight3D :public Light3D {
		public:
			Vector3D pos;
			float k_c, k_l;

			PointLight3D(const Vector3D& pos, const Vector3D& intensity, float k_c, float k_l) :Light3D(intensity), pos(pos), k_c(k_c), k_l(k_l) {
				type = 1;
			}
			ILL_DECLARE_ALIGNED_ALLOCATOR
		};

		ILL_ATTRIBUTE_ALIGNED16(class) DirectionalLight3D :public Light3D {
		public:
			Vector3D dir;
			Vector3D intensity;

			DirectionalLight3D(const Vector3D& dir, const Vector3D& intensity) :Light3D(intensity), dir(dir) {
				this->dir.normalize();
				type = 2;
			}
			ILL_DECLARE_ALIGNED_ALLOCATOR
		};
	}
}