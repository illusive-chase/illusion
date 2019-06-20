#include "SGeom3DShape.h"

void fl::geom::Cube3D::init(const Vector3D & pos, const Texture & top, const Texture & bottom, const Texture & front, const Texture & behind, const Texture & left, const Texture & right) {
	float sz = float(size >> 1);
	addPoint(Vector3D(sz, sz, sz));
	addPoint(Vector3D(sz, -sz, sz));
	addPoint(Vector3D(-sz, -sz, sz));
	addPoint(Vector3D(-sz, sz, sz));
	addPoint(Vector3D(sz, sz, -sz));
	addPoint(Vector3D(sz, -sz, -sz));
	addPoint(Vector3D(-sz, -sz, -sz));
	addPoint(Vector3D(-sz, sz, -sz));
	addSurface(0, 1, 3, front, UV(size % front.width, 0), UV(size % front.width, size % front.height), UV(0, 0));
	addSurface(1, 2, 3, front, UV(size % front.width, size % front.height), UV(0, size % front.height), UV(0, 0));
	addSurface(0, 4, 1, right, UV(0, 0), UV(size % right.width, 0), UV(0, size % right.height));
	addSurface(1, 4, 5, right, UV(0, size % right.height), UV(size % right.width, 0), UV(size % right.width, size % right.height));
	addSurface(0, 3, 7, top, UV(0, size % top.height), UV(0, 0), UV(size % top.width, 0));
	addSurface(0, 7, 4, top, UV(0, size % top.height), UV(size % top.width, 0), UV(size % top.width, size % top.height));
	addSurface(4, 6, 5, behind, UV(0, 0), UV(size % behind.width, size % behind.height), UV(0, size % behind.height));
	addSurface(4, 7, 6, behind, UV(0, 0), UV(size % behind.width, 0), UV(size % behind.width, size % behind.height));
	addSurface(3, 6, 7, left, UV(size % left.width, 0), UV(0, size % left.height), UV(0, 0));
	addSurface(3, 2, 6, left, UV(size % left.width, 0), UV(size % left.width, size % left.height), UV(0, size % left.height));
	addSurface(1, 6, 2, bottom, UV(size % bottom.width, 0), UV(0, size % bottom.height), UV(0, 0));
	addSurface(1, 5, 6, bottom, UV(size % bottom.width, 0), UV(size % bottom.width, size % bottom.height), UV(0, size % bottom.height));
}

fl::geom::Sphere3D::Sphere3D(const Vector3D & pos, const Texture & texture, int row, int col, int r) :SObject3D(pos), row(row), col(col), r(r) {
	const int width = texture.width - 1;
	const int height = texture.height - 1;

	vector<int> uv;
	vector<int> newv, oldv;
	const float theta = 3.14159265358 / row;
	const float phi = 2 * 3.14159265358 / col;


#define ID(i,j) ((j==col)?(1+((i)-1)*(col)):(1+((i)-1)*(col)+(j)))
#define UVID(i,j) (uv[((i)*(col+1)+(j))])

	addPoint(Vector3D(0.0, float(r), 0.0));
	for (int j = 0; j < col; ++j) uv.push_back(UV(int((width * j + width / 2.0) / col), 0));
	uv.push_back(UV(0, 0));

	for (int i = 1; i <= row; ++i) {
		float sin_ti = sin(theta * i);
		float cos_ti = cos(theta * i);
		for (int j = 0; j < col; ++j) {
			addPoint(Vector3D(sin_ti * r * sin(phi * j), cos_ti * r, sin_ti * r * cos(phi * j)));
			uv.push_back(UV(width * j / col, height * i / row));
		}
		uv.push_back(UV(width, height * i / row));
	}

	addPoint(Vector3D(0.0, float(-r), 0.0));
	for (int j = 0; j < col; ++j) uv.push_back(UV(int((width * j + width / 2.0) / col), height));
	uv.push_back(UV(0, 0));

	//surface
	for (int j = 0; j < col; ++j) {
		addSurface(0, ID(1, j + 1), ID(1, j), texture, UVID(0, j), UVID(1, j + 1), UVID(1, j));
		addSurface((row - 1) * col + 1, ID(row, j), ID(row, j + 1), texture, UVID(row, j), UVID(row, j), UVID(row, j + 1));
	}
	for (int i = 1; i < row; ++i) {
		for (int j = 0; j < col; ++j) {
			addSurface(ID(i, j), ID(i, j + 1), ID(i + 1, j), texture, UVID(i, j), UVID(i, j + 1), UVID(i + 1, j));
			addSurface(ID(i, j + 1), ID(i + 1, j + 1), ID(i + 1, j), texture, UVID(i, j + 1), UVID(i + 1, j + 1), UVID(i + 1, j));
		}
	}

#undef ID
#undef UVID
}

fl::geom::SQuadr3D::SQuadr3D(const Vector3D & pos, const Vector3D & width_vec, const Vector3D & height_vec, const Texture & texture) 
	:SObject3D(pos)
{
	int w = texture.width - 1, h = texture.height - 1;
	addPoint(width_vec + height_vec);
	addPoint(width_vec - height_vec);
	addPoint(-width_vec - height_vec);
	addPoint(height_vec - width_vec);
	addSurface(0, 1, 2, texture, UV(w, 0), UV(w, h), UV(0, h));
	addSurface(0, 2, 3, texture, UV(w, 0), UV(0, h), UV(0, 0));
}