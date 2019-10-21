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


// The lighting haven¡¯t been designed well yet, and the annotations here are temporarily omitted.

#include "../top_element/SPointer.h"
#include "SGeomMath.h"

namespace fl {
	namespace geom {
		ILL_ATTRIBUTE_ALIGNED16(class) Light3DImpl {
		public:
			Vector3D intensity;
			int type;
			ILL_DECLARE_ALIGNED_ALLOCATOR
			Light3DImpl(const Vector3D& intensity) : intensity(intensity), type(0) {}
			virtual ~Light3DImpl() {}
		};

		using Light3D = sptr<Light3DImpl>;
		ILL_INLINE Light3D MakeLight3D(const Vector3D& intensity) {
			return Light3D(new Light3DImpl(intensity));
		}

		class PointLight3DImpl :public Light3DImpl {
		public:
			Vector3D pos;
			scalar k_c, k_l;

			PointLight3DImpl(const Vector3D& pos, const Vector3D& intensity, scalar k_c, scalar k_l) :Light3DImpl(intensity), pos(pos), k_c(k_c), k_l(k_l) {
				type = 1;
			}
			virtual ~PointLight3DImpl() {}
		};

		using PointLight3D = sptr<PointLight3DImpl>;
		ILL_INLINE PointLight3D MakePointLight3D(const Vector3D& pos, const Vector3D& intensity, scalar k_c, scalar k_l) {
			return PointLight3D(new PointLight3DImpl(pos, intensity, k_c, k_l));
		}

		class DirectionalLight3DImpl :public Light3DImpl {
		public:
			Vector3D dir;
			Vector3D intensity;

			DirectionalLight3DImpl(const Vector3D& dir, const Vector3D& intensity) :Light3DImpl(intensity), dir(dir) {
				this->dir.normalize();
				type = 2;
			}
			virtual ~DirectionalLight3DImpl() {}
		};

		using DirectionalLight3D = sptr<DirectionalLight3DImpl>;
		ILL_INLINE DirectionalLight3D MakeDirectionalLight3D(const Vector3D& pos, const Vector3D& intensity) {
			return DirectionalLight3D(new DirectionalLight3DImpl(pos, intensity));
		}
	}
}