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
#include "SObject3D.h"

// This file encapsulates some 3D geometry based on class SObject3DImpl.
// In fact, using inheritance may not be appropriate.
// These classes actually only provide constructors that manipulate SObject objects to build some 3D geometry.

namespace fl {
	namespace geom {

		// The axis of the sphere is oriented along the y-axis by default.
		SObject3D MakeSphere3D(const Vector3D& pos, const Texture& texture, int row = 15, int col = 15, int r = 60);

		SObject3D MakeCube3D(const Vector3D& pos, const Texture& top, const Texture& bottom, const Texture& front,
			const Texture& behind, const Texture& left, const Texture& right, int size = 60);
		
		// The texture's direction can be confusing.
		// Please have a try or see the definition of function init.
		SObject3D MakeCube3D(const Vector3D& pos, const Texture& top, const Texture& bottom, const Texture& around, int size = 60);
		
		// The texture's direction can be confusing.
		// Please have a try or see the definition of function init.
		SObject3D MakeCube3D(const Vector3D& pos, const Texture& texture, int size = 60);




		/*                 ^ height_vec
		 *                 |
		 *                 |
		 *                 o------------------> width_vec
		 *
		 */
		// It builds a Parallelogram by two direction vectors, as shown above.
		SObject3D MakeSQuadr3D(const Vector3D& pos, const Vector3D& width_vec, const Vector3D& height_vec, const Texture& texture);
	}
}