/*
MIT License

Copyright (c) 2019 illusive-chase

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*/
#pragma once
#include "PForce.h"
#include "../top_element/SEvent.h"

namespace fl {
	namespace physics {


		// The function PhaseImpl::framing should be add to stage.frameEventListener(See Stage.h),
		// or it will not update per frame.
		// Class PhaseImpl is like a physical world and it manages all the physical process.
		// It provides interfaces for adding physical objects.
		class PhaseImpl {

		public:
			PFGravity g_gravity;
			PFResistance g_resistance;
			PFElastic g_elastic;
			PFCollision g_collision;
			std::vector<PObject3D> objs;
			
			PhaseImpl(scalar gravity_c, scalar resistance_c) :g_gravity(gravity_c), g_resistance(resistance_c) {}

			ILL_INLINE void addObject(PObject3D obj, bool collisible, bool dynamic = true) {
				objs.push_back(obj);
				g_gravity.addObject(obj);
				g_resistance.addObject(obj);
				if (collisible) g_collision.addObject(obj, dynamic);
			}

			ILL_INLINE void framing(events::FrameEvent e) {
				for (PObject3D obj : objs) obj->framing();
				g_elastic.apply();
				g_resistance.apply();
				g_gravity.apply();
				g_collision.apply();
			}

		};

		using Phase = sptr<PhaseImpl>;
		ILL_INLINE Phase MakePhase(scalar gravity_c, scalar resistance_c) {
			return Phase(new PhaseImpl(gravity_c, resistance_c));
		}

	}
}