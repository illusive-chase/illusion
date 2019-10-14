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



static fl::geom::SObject3D init(int size, const fl::geom::Vector3D & pos, const fl::geom::Texture & top, const fl::geom::Texture & bottom, const fl::geom::Texture & front,
	const fl::geom::Texture & behind, const fl::geom::Texture & left, const fl::geom::Texture & right)
{
	using namespace fl::geom;
	using namespace fl;
	scalar sz = scalar(size >> 1);
	SObject3D ptr = MakeSObject3D(pos);
	ptr->addPoint(Vector3D(sz, sz, sz));
	ptr->addPoint(Vector3D(sz, -sz, sz));
	ptr->addPoint(Vector3D(-sz, -sz, sz));
	ptr->addPoint(Vector3D(-sz, sz, sz));
	ptr->addPoint(Vector3D(sz, sz, -sz));
	ptr->addPoint(Vector3D(sz, -sz, -sz));
	ptr->addPoint(Vector3D(-sz, -sz, -sz));
	ptr->addPoint(Vector3D(-sz, sz, -sz));
	ptr->addSurface(0, 1, 3, front, UV(size % front.width, 0), UV(size % front.width, size % front.height), UV(0, 0));
	ptr->addSurface(1, 2, 3, front, UV(size % front.width, size % front.height), UV(0, size % front.height), UV(0, 0));
	ptr->addSurface(0, 4, 1, right, UV(0, 0), UV(size % right.width, 0), UV(0, size % right.height));
	ptr->addSurface(1, 4, 5, right, UV(0, size % right.height), UV(size % right.width, 0), UV(size % right.width, size % right.height));
	ptr->addSurface(0, 3, 7, top, UV(0, size % top.height), UV(0, 0), UV(size % top.width, 0));
	ptr->addSurface(0, 7, 4, top, UV(0, size % top.height), UV(size % top.width, 0), UV(size % top.width, size % top.height));
	ptr->addSurface(4, 6, 5, behind, UV(0, 0), UV(size % behind.width, size % behind.height), UV(0, size % behind.height));
	ptr->addSurface(4, 7, 6, behind, UV(0, 0), UV(size % behind.width, 0), UV(size % behind.width, size % behind.height));
	ptr->addSurface(3, 6, 7, left, UV(size % left.width, 0), UV(0, size % left.height), UV(0, 0));
	ptr->addSurface(3, 2, 6, left, UV(size % left.width, 0), UV(size % left.width, size % left.height), UV(0, size % left.height));
	ptr->addSurface(1, 6, 2, bottom, UV(size % bottom.width, 0), UV(0, size % bottom.height), UV(0, 0));
	ptr->addSurface(1, 5, 6, bottom, UV(size % bottom.width, 0), UV(size % bottom.width, size % bottom.height), UV(0, size % bottom.height));
	return ptr;
}

fl::geom::SObject3D fl::geom::MakeSphere3D(const fl::geom::Vector3D & pos, const fl::geom::Texture & texture, int row, int col, int r)
{
	using namespace fl::geom;
	using namespace fl;
	SObject3D ptr = MakeSObject3D(pos);
	const int width = texture.width - 1;
	const int height = texture.height - 1;

	std::vector<int> uv;
	std::vector<int> newv, oldv;
	const scalar theta = PI / row;
	const scalar phi = 2 * PI / col;


#define ID(i,j) ((j==col)?(1+((i)-1)*(col)):(1+((i)-1)*(col)+(j)))
#define UVID(i,j) (uv[((i)*(col+1)+(j))])

	ptr->addPoint(Vector3D(0.0, scalar(r), 0.0));
	for (int j = 0; j < col; ++j) uv.push_back(UV(int((width * j + width / 2.0) / col), 0));
	uv.push_back(UV(0, 0));

	for (int i = 1; i <= row; ++i) {
		scalar sin_ti = sin(theta * i);
		scalar cos_ti = cos(theta * i);
		for (int j = 0; j < col; ++j) {
			ptr->addPoint(Vector3D(sin_ti * r * sin(phi * j), cos_ti * r, sin_ti * r * cos(phi * j)));
			uv.push_back(UV(width * j / col, height * i / row));
		}
		uv.push_back(UV(width, height * i / row));
	}

	ptr->addPoint(Vector3D(0.0, scalar(-r), 0.0));
	for (int j = 0; j < col; ++j) uv.push_back(UV(int((width * j + width / 2.0) / col), height));
	uv.push_back(UV(0, 0));

	//surface
	for (int j = 0; j < col; ++j) {
		ptr->addSurface(0, ID(1, j + 1), ID(1, j), texture, UVID(0, j), UVID(1, j + 1), UVID(1, j));
		ptr->addSurface((row - 1) * col + 1, ID(row, j), ID(row, j + 1), texture, UVID(row, j), UVID(row, j), UVID(row, j + 1));
	}
	for (int i = 1; i < row; ++i) {
		for (int j = 0; j < col; ++j) {
			ptr->addSurface(ID(i, j), ID(i, j + 1), ID(i + 1, j), texture, UVID(i, j), UVID(i, j + 1), UVID(i + 1, j));
			ptr->addSurface(ID(i, j + 1), ID(i + 1, j + 1), ID(i + 1, j), texture, UVID(i, j + 1), UVID(i + 1, j + 1), UVID(i + 1, j));
		}
	}

#undef ID
#undef UVID

	return ptr;
}

fl::geom::SObject3D fl::geom::MakeSQuadr3D(const fl::geom::Vector3D & pos, const fl::geom::Vector3D & width_vec, const fl::geom::Vector3D & height_vec, const fl::geom::Texture & texture)
{
	using namespace fl::geom;
	using namespace fl;
	int w = texture.width - 1, h = texture.height - 1;
	SObject3D ptr = MakeSObject3D(pos);
	ptr->addPoint(width_vec + height_vec);
	ptr->addPoint(width_vec - height_vec);
	ptr->addPoint(-width_vec - height_vec);
	ptr->addPoint(height_vec - width_vec);
	ptr->addSurface(0, 1, 2, texture, UV(w, 0), UV(w, h), UV(0, h));
	ptr->addSurface(0, 2, 3, texture, UV(w, 0), UV(0, h), UV(0, 0));
	return ptr;
}

fl::geom::SObject3D fl::geom::MakeCube3D(const fl::geom::Vector3D & pos, const fl::geom::Texture & top, const fl::geom::Texture & bottom,
	const fl::geom::Texture & front, const fl::geom::Texture & behind, const fl::geom::Texture & left, const fl::geom::Texture & right, int size)
{
	using namespace fl::geom;
	using namespace fl;
	return init(size, pos, top, bottom, front, behind, left, right);
}

fl::geom::SObject3D fl::geom::MakeCube3D(const fl::geom::Vector3D & pos, const fl::geom::Texture & top, const fl::geom::Texture & bottom,
	const Texture & around, int size)
{
	using namespace fl::geom;
	using namespace fl;
	return init(size, pos, top, bottom, around, around, around, around);
}

fl::geom::SObject3D fl::geom::MakeCube3D(const fl::geom::Vector3D & pos, const fl::geom::Texture & texture, int size) {
	using namespace fl::geom;
	using namespace fl;
	return init(size, pos, texture, texture, texture, texture, texture, texture);
}
