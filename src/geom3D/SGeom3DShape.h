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

// This file encapsulates some 3D geometry based on class SObject3D.
// In fact, using inheritance may not be appropriate.
// These classes actually only provide constructors that manipulate SObject objects to build some 3D geometry.

namespace fl {
	namespace geom {

		// Class Sphere3D inherits class AutoPtr indirectly, which means it must be allocated on the heap.
		// The axis of the sphere is oriented along the y-axis by default.
		class Sphere3D :public SObject3D {
		public:
			// the number of rows and cols of triangle fragments
			const int row, col;
			const int r; // radius
			Sphere3D(const Vector3D& pos, const Texture& texture, int row = 15, int col = 15, int r = 60);
		};

		// Class Cube3D inherits class AutoPtr indirectly, which means it must be allocated on the heap.
		class Cube3D :public SObject3D {
		public:
			const int size; // side length

			// The texture's direction can be confusing.
			// Please have a try or see the definition of function init.
			Cube3D(const Vector3D& pos, const Texture& top, const Texture& bottom, const Texture& front,
				const Texture& behind, const Texture& left, const Texture& right, int size = 60) :SObject3D(pos), size(size) {
				init(pos, top, bottom, front, behind, left, right);
			}
			// The texture's direction can be confusing.
			// Please have a try or see the definition of function init.
			Cube3D(const Vector3D& pos, const Texture& top, const Texture& bottom, const Texture& around, int size = 60) :SObject3D(pos), size(size) {
				init(pos, top, bottom, around, around, around, around);
			}
			// The texture's direction can be confusing.
			// Please have a try or see the definition of function init.
			Cube3D(const Vector3D& pos, const Texture& texture, int size = 60) :SObject3D(pos), size(size) {
				init(pos, texture, texture, texture, texture, texture, texture);
			}
			
		private:
			void init(const Vector3D& pos, const Texture& top, const Texture& bottom, const Texture& front,
				const Texture& behind, const Texture& left, const Texture& right);
		};




		/*                 ^ height_vec
		 *                 |
		 *                 |
		 *                 o------------------> width_vec
		 *
		 */
		// Class Cube3D inherits class AutoPtr indirectly, which means it must be allocated on the heap.
		// It builds a Parallelogram by two direction vectors, as shown above.
		class SQuadr3D : public SObject3D {
		public:
			SQuadr3D(const Vector3D& pos, const Vector3D& width_vec, const Vector3D& height_vec, const Texture& texture);
		};
	}
}