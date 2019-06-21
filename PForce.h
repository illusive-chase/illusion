#pragma once
#include "PObject3D.h"
namespace fl {
	namespace physics{
	
		class PFGravity {
		private:
			scalar g;
		public:
			explicit PFGravity(scalar g) :g(g) {}
			void operator()(PObject3D* obj) { obj->acc += geom::Vector3D(0, -g, 0); }
		};

		class PFResistance {
		private:
			scalar k;
		public:
			explicit PFResistance(scalar k) :k(k) {}
			void operator()(PObject3D* obj) { obj->acc -= obj->vel * (k / obj->mass); }
		};

		class PFElastic {
		private:
			scalar k;
			scalar x0;
			PObject3D* other;
		public:
			PFElastic(scalar k, scalar x0, PObject3D* other) :k(k), x0(x0), other(other) {}
			void operator()(PObject3D* obj) {
				Vector3D dis(other->pos);
				dis -= obj->pos;
				obj->acc += dis * ((1 - x0 / dis.mod()) * k / obj->mass);
			}
		};

	}
}