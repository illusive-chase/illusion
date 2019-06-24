#pragma once
#include "PForce.h"
#include "SEvent.h"

namespace fl {
	namespace physics {

		class Phase :public AutoPtr {

		public:
			PFGravity g_gravity;
			PFResistance g_resistance;
			PFElastic g_elastic;
			PFCollision g_collision;
			std::vector<PObject3D*> objs;
			
			Phase(scalar gravity_c, scalar resistance_c) :g_gravity(gravity_c), g_resistance(resistance_c) {}
			~Phase() { for (PObject3D* obj : objs) delete obj; }

			ILL_INLINE void addObject(PObject3D* obj, bool collisible, bool dynamic = true) {
				objs.push_back(obj);
				g_gravity.addObject(obj);
				g_resistance.addObject(obj);
				if (collisible) g_collision.addObject(obj, dynamic);
			}

			ILL_INLINE void framing(events::FrameEvent e) {
				for (PObject3D* obj : objs) obj->framing();
				g_collision.apply();
				g_elastic.apply();
				g_resistance.apply();
				g_gravity.apply();
			}

		};

	}
}