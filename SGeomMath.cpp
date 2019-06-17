#include "SGeomMath.h"


fl::geom::LerpX::LerpX(const LerpY & sa, const LerpY & sb)
	: x(sa.x), z(sa.z), u(sa.u), v(sa.v), r(sa.r), g(sa.g), b(sa.b), dx(1.0 / (sb.x - x)), dz((sb.z - z) * dx),
	du((sb.u - u) * dx), dv((sb.v - v) * dx), dr((sb.r - r) * dx), dg((sb.g - g) * dx), db((sb.b - b) * dx) {
}

void fl::geom::LerpX::start(int ax, float sample_x) {
	move(ax - x + sample_x);
}

void fl::geom::LerpX::move() {
	z += dz;
	u += du;
	v += dv;
	r += dr;
	b += db;
	g += dg;
}

void fl::geom::LerpX::move(float step) {
	z += dz * step;
	u += du * step;
	v += dv * step;
	r += dr * step;
	b += db * step;
	g += dg * step;
}

fl::geom::LerpY::LerpY(const Shadee & sa, const Shadee & sb)
	:x(sa.x), y(sa.y), z(1.0 / sa.z), u(sa.u * z), v(sa.v * z), r(sa.r * z), g(sa.g * z), b(sa.b * z), dy(1.0 / (sb.y - y)),
	dx((sb.x - x) * dy), dz((1.0 / sb.z - z) * dy), du((sb.u / sb.z - u) * dy), dv((sb.v / sb.z - v) * dy),
	dr((sb.r / sb.z - r) * dy), dg((sb.g / sb.z - g) * dy), db((sb.b / sb.z - b) * dy) {
}

fl::geom::LerpY::LerpY(const Shadee & sa, const Shadee & sb, float step)
	: x(sa.x), y(sa.y), z(1.0 / sa.z), u(sa.u * z), v(sa.v * z), r(sa.r * z), g(sa.g * z), b(sa.b * z), dy(step / (sb.y - y)),
	dx((sb.x - x) * dy), dz((1.0 / sb.z - z) * dy), du((sb.u / sb.z - u) * dy), dv((sb.v / sb.z - v) * dy),
	dr((sb.r / sb.z - r) * dy), dg((sb.g / sb.z - g) * dy), db((sb.b / sb.z - b) * dy) {
}

void fl::geom::LerpY::reset(const Shadee & sa) {
	x = sa.x;
	z = 1.0 / sa.z;
	u = sa.u * z;
	v = sa.v * z;
	r = sa.r * z;
	g = sa.g * z;
	b = sa.b * z;
}

void fl::geom::LerpY::start(int ay, float sample_y) {
	move(ay - y + sample_y);
}

void fl::geom::LerpY::move() {
	x += dx;
	z += dz;
	u += du;
	v += dv;
	r += dr;
	b += db;
	g += dg;
}

void fl::geom::LerpY::move(float step) {
	x += dx * step;
	z += dz * step;
	u += du * step;
	v += dv * step;
	r += dr * step;
	b += db * step;
	g += dg * step;
}
