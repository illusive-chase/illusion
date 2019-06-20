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

		class PFElastic {
		private:
			float k;
			float x0;
			PObject3D* other;
		public:
			PFElastic(float k, float x0, PObject3D* other) :k(k), x0(x0), other(other) {}
			void operator()(PObject3D* obj) {
				Vector3D dis(other->pos);
				dis -= obj->pos;
				obj->acc += dis * ((1 - x0 / dis.mod()) * k / obj->mass);
			}
		};

	}
}