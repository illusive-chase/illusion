#include "SObject3D.h"


int fl::geom::SObject3D::addPoint(const Vector3D & p, const Vector3D& n) {
	vertex.push_back(p);
	normal.push_back(n);
	normal.back().normalize();
	return (int)vertex.size() - 1;
}

int fl::geom::SObject3D::addPoint(const Vector3D& p) {
	return addPoint(p, p);
}

int fl::geom::SObject3D::addSurface(int pa, int pb, int pc, const Texture& texture, int uva, int uvb, int uvc) {
	surface.push_back(Surface3D(pa, pb, pc, texture, uva, uvb, uvc));
	return (int)surface.size() - 1;
}

void fl::geom::SObject3D::rotateX(const Rad& rad) {
	for (Vector3D& p : vertex) p.rotateX(rad);
	for (Vector3D& p : normal) p.rotateX(rad);
	m_x.rotateX(rad);
	m_y.rotateX(rad);
	m_z.rotateX(rad);
}

void fl::geom::SObject3D::rotateY(const Rad& rad) {
	for (Vector3D& p : vertex) p.rotateY(rad);
	for (Vector3D& p : normal) p.rotateY(rad);
	m_x.rotateY(rad);
	m_y.rotateY(rad);
	m_z.rotateY(rad);
}

void fl::geom::SObject3D::rotateZ(const Rad& rad) {
	for (Vector3D& p : vertex) p.rotateZ(rad);
	for (Vector3D& p : normal) p.rotateZ(rad);
	m_x.rotateZ(rad);
	m_y.rotateZ(rad);
	m_z.rotateZ(rad);
}

void fl::geom::SObject3D::scale(float factor) {
	for (Vector3D& p : vertex) p *= factor;
}

void fl::geom::SObject3D::rotateX(const Rad& rad, const Vector3D& refv) {
	pos -= refv;
	pos.rotateX(rad);
	pos += refv;
	rotateX(rad);
}

void fl::geom::SObject3D::rotateY(const Rad& rad, const Vector3D& refv) {
	pos -= refv;
	pos.rotateY(rad);
	pos += refv;
	rotateY(rad);
}

void fl::geom::SObject3D::rotateZ(const Rad& rad, const Vector3D& refv) {
	pos -= refv;
	pos.rotateZ(rad);
	pos += refv;
	rotateZ(rad);
}

void fl::geom::SObject3D::scale(float factor, const Vector3D& refv) {
	pos *= factor;
	pos += refv * (1.0 - factor);
	scale(factor);
}
