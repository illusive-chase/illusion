#pragma once
#include "PObject3D.h"
namespace fl {
	namespace physics{
	
		class PFGravity {
		private:
			float g;
		public:
			explicit PFGravity(float g) :g(g) {}
			void operator()(PObject3D* obj) { obj->acc += geom::Vector3D(0, -g, 0); }
		};

		class PFResistance {
		private:
			float k;
		public:
			explicit PFResistance(float k) :k(k) {}
			void operator()(PObject3D* obj) { obj->acc -= obj->vel * (k / obj->mass); }
		};

	}
}