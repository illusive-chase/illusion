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
#include "PObject3D.h"
#include "PBroadCollision.h"
#include "PNarrowCollision.h"

namespace fl {
	namespace physics{
	
#ifdef USE_FORCE_GENERATOR
		class PFGravity {
		private:
			scalar g;
		public:
			explicit PFGravity(scalar g) :g(g) {}
			void operator()(PObject3D* obj) { obj->acc += geom::Vector3D(scalar(0), -g, scalar(0)); }
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

#endif

		class PFGravity {
		private:
			scalar g;
			std::list<PObject3D*> objs;
		public:
			bool enable;
			explicit PFGravity(scalar g) :g(g), enable(true) {}
			ILL_INLINE void addObject(PObject3D* obj) { if (obj->mass) objs.push_back(obj); }
			ILL_INLINE void apply() { 
				if (enable) {
					for (PObject3D* obj : objs) obj->acc += geom::Vector3D(scalar(0), -g, scalar(0));
				}
			}
		};

		class PFResistance {
		private:
			scalar k;
			std::list<PObject3D*> objs;
		public:
			bool enable;
			explicit PFResistance(scalar k) :k(k), enable(true) {}
			ILL_INLINE void addObject(PObject3D* obj) { if (obj->mass) objs.push_back(obj); }
			ILL_INLINE void apply() { if (enable) for (PObject3D* obj : objs) obj->acc -= obj->vel * (k / obj->mass); }
		};

		class PFElastic {
		private:
			struct elastic_trait {
				scalar k;
				scalar x0;
				PObject3D* a;
				PObject3D* b;
			};

			std::list<elastic_trait> obj_pairs;
		public:
			bool enable;
			PFElastic() :enable(true) {}
			ILL_INLINE void addObject(PObject3D* a, PObject3D* b, scalar k, scalar x0) { 
				if (a->mass || b->mass) obj_pairs.push_back(elastic_trait{ k,x0,a,b });
			}
			ILL_INLINE void apply() {
				if (enable) {
					for (elastic_trait trait : obj_pairs) {
						Vector3D dis = trait.b->pos - trait.a->pos;
						dis *= ((1 - trait.x0 / dis.mod()) * trait.k);
						if(trait.a->mass) trait.a->acc += dis * (scalar(1) / trait.a->mass);
						if(trait.b->mass) trait.b->acc -= dis * (scalar(1) / trait.b->mass);
					}
				}
			}
		};

		class PFCollision {
		private:
			PBroadCollision pbc;
			static constexpr scalar COLLISION_SEPARATE_ACC_WHEN_CREATED = scalar(0.5);

		public:
			bool enable;
			PFCollision() :enable(true) {}
			ILL_INLINE void addObject(PObject3D* obj, bool dynamic) {
				std::stack<void*> stk;
				if (dynamic) pbc.createDynamicProxyInstant(stk, obj->aabb, obj);
				else pbc.createStaticProxyInstant(stk, obj->aabb, obj);
				while (!stk.empty()) {
					PObject3D* p = reinterpret_cast<PObject3D*>(stk.top());
					PNarrowCollision::collide(obj, p);
					stk.pop();
				}
			}
			ILL_INLINE void apply() {
				std::stack<void*> stk;
				pbc.cleanup();
				for (int i = (int)pbc.leaves.size() - 1; i >= 0; --i) {
					PObject3D* obj = reinterpret_cast<PObject3D*>(pbc.leaves[i].leaf->obj);
					pbc.setProxyInstant(stk, i, obj->aabb, obj->vel);
					while (!stk.empty()) {
						PObject3D* p = reinterpret_cast<PObject3D*>(stk.top());
						PNarrowCollision::collide(obj, p);
						stk.pop();
					}
				}
			}

			
		};

	}
}