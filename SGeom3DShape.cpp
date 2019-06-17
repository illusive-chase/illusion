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