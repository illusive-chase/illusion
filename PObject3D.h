#pragma once
#include "SPointer.h"
#include "SGeomMath.h"
#include "SEvent.h"

namespace fl {
	namespace physics {

		class PObject3D :public AutoPtr {
		private:
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
			const scalar mass;
			scalar recovery;
			geom::Vector3D pos;
			geom::Vector3D vel;
			geom::Vector3D acc;
			geom::AxisAlignedBoundingBox aabb;
			
			
			PObject3D(scalar mass, const geom::Vector3D& pos, const geom::AxisAlignedBoundingBox& aabb,
				scalar recovery = scalar(0))
				:mass(mass), pos(pos), aabb(aabb), recovery(recovery) {
			}

#if USE_FORCE_GENERATOR
			template<typename ExactForceType> void addForce(ExactForceType f) { force.add(f); }
#endif

			ILL_INLINE void framing() {
				pos += vel;
				aabb.mx += vel;
				aabb.mi += vel;
				vel += acc;
				printf("pos : %f,%f,%f\nvel : %f,%f,%f\nacc : %f,%f,%f\n\n", pos.x, pos.y, pos.z, vel.x, vel.y, vel.z, acc.x, acc.y, acc.z);
				acc = geom::Vector3D();
#if USE_FORCE_GENERATOR
				if (mass) force(this);
#endif
			}


		};

		class PSphere :public PObject3D {
		public:
			scalar radius;

			PSphere(scalar mass, const geom::Vector3D& pos, scalar radius, scalar recovery)
				:PObject3D(mass, pos, geom::AxisAlignedBoundingBox(
					geom::Vector3D(-radius, -radius, -radius), geom::Vector3D(radius, radius, radius)
				), recovery), radius(radius) {
			}

		};

		
	}
}