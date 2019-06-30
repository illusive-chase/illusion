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
#include "SPointer.h"
#include "SGeomMath.h"
#include "Struct.h"

namespace fl {
	namespace physics {

		class PObject3D :public AutoPtr {
		private:

			// Once, physical force was implemented with local generators.
			// Now, force is implemented with global manager. See PForce.h.
#ifdef USE_FORCE_GENERATOR
			class PForceGenerator {
			private:
				class PForceGeneratorBase {
				public:
					virtual void Execute(PObject3D* obj) = 0;
				};

				template<typename ExactForceType>
				class PForceGeneratorImpl :public PForceGeneratorBase, public ExactForceType {
				public:
					PForceGeneratorImpl(const ExactForceType& other) :PForceGeneratorBase(), ExactForceType(other) {}
					void Execute(PObject3D* obj) { ExactForceType::operator()(obj); }
				};

				std::list<PForceGeneratorBase*> base;

			public:

				template<typename ExactForceType>
				void add(const ExactForceType& f) { base.push_back(new PForceGeneratorImpl<ExactForceType>(f)); }
				void operator()(PObject3D* obj) { for (auto it : base) it->Execute(obj); }
				~PForceGenerator() { for (auto it : base) delete it; }
			} force;
#endif

		public:
			const scalar mass; // 0 is used to presented infinity.
			scalar recovery; // The recovery coefficient in the collision of objects A and B is equal to A.recovery * B.recovery.
			geom::Vector3D pos;
			geom::Vector3D vel;
			geom::Vector3D acc;
			geom::AxisAlignedBoundingBox aabb;
			
			
			PObject3D(scalar mass, const geom::Vector3D& pos, const geom::AxisAlignedBoundingBox& aabb,
				scalar recovery = scalar(0))
				:mass(mass), pos(pos), aabb(aabb.mi + pos, aabb.mx + pos), recovery(recovery) {
			}

#if USE_FORCE_GENERATOR
			template<typename ExactForceType> void addForce(ExactForceType f) { force.add(f); }
#endif

			// Update pos first, then vel, finally acc.
			// That's very important for collision detection.
			ILL_INLINE void framing() {
				pos += vel;
				aabb.mx += vel;
				aabb.mi += vel;
				vel += acc;
				acc = geom::Vector3D();
#if USE_FORCE_GENERATOR
				if (mass) force(this);
#endif
			}

			unsigned virtual uid() const = 0; // All of the definition of function uid is in file PObject3D.cpp.


		};

		class PSphere :public PObject3D {
		public:
			scalar radius;

			PSphere(scalar mass, const geom::Vector3D& pos, scalar radius, scalar recovery)
				:PObject3D(mass, pos, geom::AxisAlignedBoundingBox(
					geom::Vector3D(-radius, -radius, -radius),
					geom::Vector3D(radius, radius, radius)
				), recovery), radius(radius) {
			}

			unsigned uid() const;

		};

		class Platform :public PObject3D {
		public:

			Platform(const geom::Vector3D& pos, scalar radius, scalar recovery)
				:PObject3D(scalar(0), pos, geom::AxisAlignedBoundingBox(
					geom::Vector3D(-radius, -radius, -radius),
					geom::Vector3D(radius, radius, radius)
				), recovery) {
			}

			unsigned uid() const;

		};

		using PShapeArray = TypeTrait::TypeArray<PSphere>; // Here is used TMP. See class TypeTrait in Struct.h.
    }
}