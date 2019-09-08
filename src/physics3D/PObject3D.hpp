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
#include "../top_element/SPointer.h"
#include "../geom3D/SGeomMath.h"
#include "../top_element/Struct.h"

namespace fl {
	namespace physics {

		class PObject3DImpl {

		public:
			const scalar mass; // 0 is used to presented infinity.
			scalar recovery; // The recovery coefficient in the collision of objects A and B is equal to A.recovery * B.recovery.
			geom::Vector3D pos;
			geom::Vector3D vel;
			geom::Vector3D acc;
			geom::AxisAlignedBoundingBox aabb;


			PObject3DImpl(scalar mass, const geom::Vector3D& pos, const geom::AxisAlignedBoundingBox& aabb,
						  scalar recovery = scalar(0))
				:mass(mass), pos(pos), aabb(aabb.mi + pos, aabb.mx + pos), recovery(recovery) {
			}

			virtual ~PObject3DImpl() {}

			// Update pos first, then vel, finally acc.
			// That's very important for collision detection.
			ILL_INLINE void framing() {
				pos += vel;
				aabb.mx += vel;
				aabb.mi += vel;
				vel += acc;
				acc = geom::Vector3D();
			}

			unsigned virtual uid() const = 0; // All of the definition of function uid is in file PObject3DImpl.cpp.


		};

		using PObject3D = sptr<PObject3DImpl>;

		class PSphereImpl :public PObject3DImpl {
		public:
			scalar radius;

			PSphereImpl(scalar mass, const geom::Vector3D& pos, scalar radius, scalar recovery)
				:PObject3DImpl(mass, pos, geom::AxisAlignedBoundingBox(
					geom::Vector3D(-radius, -radius, -radius),
					geom::Vector3D(radius, radius, radius)
				), recovery), radius(radius) {
			}
			~PSphereImpl() {}
			unsigned uid() const override;

		};

		using PSphere = sptr<PSphereImpl>;
		ILL_INLINE PSphere MakePSphere(scalar mass, const geom::Vector3D& pos, scalar radius, scalar recovery) {
			return PSphere(new PSphereImpl(mass, pos, radius, recovery));
		}

		class PQuadImpl :public PObject3DImpl {
		public:
			geom::Vector3D sa, sb, sn;
			PQuadImpl(const geom::Vector3D& pos, const geom::Vector3D& sa, const geom::Vector3D& sb, scalar recovery)
				:PObject3DImpl(scalar(0), pos, geom::AxisAlignedBoundingBox(), recovery), sa(sa), sb(sb),
				sn(normalVector(sa, sb))
			{
				sn.normalize();
				// There might be a faster method using SSE.
				if (sa.x > 0) {
					if (sb.x < 0) aabb.mi.x = sb.x - sa.x;
					else aabb.mi.x = -sa.x - sb.x;
				} else {
					if (sb.x < 0) aabb.mi.x = sa.x + sb.x;
					else aabb.mi.x = sa.x - sb.x;
				}
				if (sa.y > 0) {
					if (sb.y < 0) aabb.mi.y = sb.y - sa.y;
					else aabb.mi.y = -sa.y - sb.y;
				} else {
					if (sb.y < 0) aabb.mi.y = sa.y + sb.y;
					else aabb.mi.y = sa.y - sb.y;
				}
				if (sa.z > 0) {
					if (sb.z < 0) aabb.mi.z = sb.z - sa.z;
					else aabb.mi.z = -sa.z - sb.z;
				} else {
					if (sb.z < 0) aabb.mi.z = sa.z + sb.z;
					else aabb.mi.z = sa.z - sb.z;
				}
				aabb.mx = pos - aabb.mi;
				aabb.mi += pos;
			}
			~PQuadImpl() {}
			unsigned uid() const override;

		};

		using PQuad = sptr<PQuadImpl>;
		ILL_INLINE PQuad MakePQuad(const geom::Vector3D& pos, const geom::Vector3D& sa, const geom::Vector3D& sb, scalar recovery) {
			return PQuad(new PQuadImpl(pos, sa, sb, recovery));
		}

		using PShapeArray = TypeTrait::TypeArray<PSphere, PQuad>; // Here used is TMP. See class TypeTrait in Struct.h.
	}
}

#define DECLARE_UID(shape) unsigned shape::uid() const { return fl::physics::PShapeArray::getIndex<fl::sptr<shape>>(); }

DECLARE_UID(fl::physics::PSphereImpl)
DECLARE_UID(fl::physics::PQuadImpl)

#undef DECLARE_UID