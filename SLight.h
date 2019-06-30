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

#include "SPointer.h"
#include "SGeomMath.h"

namespace fl {
	namespace geom {
		class Light3D :public AutoPtr {
		public:
			Vector3D intensity;
			int type;
			Light3D(const Vector3D& intensity) :AutoPtr(), intensity(intensity), type(0) {}
			virtual ~Light3D() {}

			
		};

		class PointLight3D :public Light3D {
		public:
			Vector3D pos;
			scalar k_c, k_l;

			PointLight3D(const Vector3D& pos, const Vector3D& intensity, scalar k_c, scalar k_l) :Light3D(intensity), pos(pos), k_c(k_c), k_l(k_l) {
				type = 1;
			}
			
		};

		class DirectionalLight3D :public Light3D {
		public:
			Vector3D dir;
			Vector3D intensity;

			DirectionalLight3D(const Vector3D& dir, const Vector3D& intensity) :Light3D(intensity), dir(dir) {
				this->dir.normalize();
				type = 2;
			}
			
		};
	}
}