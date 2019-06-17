#pragma once
#include "SObject3D.h"

namespace fl {
	namespace physics {

		class PObject3D :public AutoPtr {
		public:
			geom::Vector3D pos;
			geom::Vector3D v;
			const enum Type {
				SPHERE,
				CUBE
			} type;
			PObject3D(Type type, const geom::Vector3D& pos) :pos(pos), type(type) {}
			virtual bool hitTest(PObject3D* other, const geom::Vector3D& dir) = 0;
		};

		class PSphere3D :public PObject3D {
		public:
			float r;

			PSphere3D(const geom::Vector3D& pos, float r) :PObject3D(SPHERE, pos), r(r) {}

			bool hitTest(PObject3D* other, const geom::Vector3D& dir) {

			}
		};

		
	}
}