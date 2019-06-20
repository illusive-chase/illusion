#pragma once
#include "SPointer.h"
#include "SGeomMath.h"

namespace fl {
	namespace physics {

		class PObject3D :public AutoPtr {
		private:
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

		public:
			float mass;
			geom::Vector3D pos;
			geom::Vector3D vel;
			geom::Vector3D acc;
			
			explicit PObject3D(float mass, const geom::Vector3D& pos) :mass(mass), pos(pos), vel(), acc() {}
			template<typename ExactForceType> void addForce(ExactForceType f) { force.add(f); }
			void framing(float duration) {
				pos += vel * duration;
				vel += acc * duration;
				acc = Vector3D();
				if(mass) force(this);
			}
			void framing() {
				pos += vel;
				vel += acc;
				acc = Vector3D();
				if (mass) force(this);
			}


		};

		
	}
}