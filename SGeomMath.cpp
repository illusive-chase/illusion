#include "SGeomMath.h"


fl::geom::LerpX::LerpX(const LerpY & sa, const LerpY & sb)
#ifndef ILL_SSE_IN_API
	: x(sa.x), y(sa.y), z(sa.z), u(sa.u), v(sa.v), r(sa.r), g(sa.g), b(sa.b), dx(1.0 / (sb.x - x)), dy((sb.y - y) * dx), dz((sb.z - z) * dx),
	du((sb.u - u) * dx), dv((sb.v - v) * dx), dr((sb.r - r) * dx), dg((sb.g - g) * dx), db((sb.b - b) * dx) 
#endif
{
#ifdef ILL_SSE_IN_API
	m_xyuv = sa.m_xyuv;
	m_rgbz = sa.m_rgbz;
	m_dxyuv = _mm_sub_ps(sb.m_xyuv, sa.m_xyuv);
	m_drgbz = _mm_sub_ps(sb.m_rgbz, sa.m_rgbz);
	f4 mask = _mm_shuffle_ps(m_dxyuv, m_dxyuv, 0x0); //_MM_SHUFFLE(0, 0, 0, 0)
	m_dxyuv = _mm_div_ps(m_dxyuv, mask);
	m_drgbz = _mm_div_ps(m_drgbz, mask);
#endif
}

fl::geom::LerpY::LerpY(const Shadee & sa, const Shadee & sb)
#ifndef ILL_SSE_IN_API
	:x(sa.x), y(sa.y), z(sa.z), u(sa.u), v(sa.v), r(sa.r), g(sa.g), b(sa.b), dy(1.0 / (sb.y - y)),
	dx((sb.x - x) / (sb.y - y)), dz((sb.z - z) * dy), du((sb.u - u) * dy), dv((sb.v - v) * dy),
	dr((sb.r - r) * dy), dg((sb.g - g) * dy), db((sb.b - b) * dy) 
#endif
{
#ifdef ILL_SSE_IN_API
	m_xyuv = sa.m_xyuv;
	m_rgbz = sa.m_rgbz;
	m_dxyuv = _mm_sub_ps(sb.m_xyuv, sa.m_xyuv);
	m_drgbz = _mm_sub_ps(sb.m_rgbz, sa.m_rgbz);
	f4 mask = _mm_shuffle_ps(m_dxyuv, m_dxyuv, 0x55); //_MM_SHUFFLE(1, 1, 1, 1)
	m_dxyuv = _mm_div_ps(m_dxyuv, mask);
	m_drgbz = _mm_div_ps(m_drgbz, mask);
#endif
}

fl::geom::LerpY::LerpY(const Shadee & sa, const Shadee & sb, const float& step)
#ifndef ILL_SSE_IN_API
	: x(sa.x), y(sa.y), z(sa.z), u(sa.u), v(sa.v), r(sa.r), g(sa.g), b(sa.b), dy(step / (sb.y - y)),
	dx((sb.x - x) * (step / (sb.y - y))), dz((sb.z - z) * dy), du((sb.u - u) * dy), dv((sb.v - v) * dy),
	dr((sb.r - r) * dy), dg((sb.g - g) * dy), db((sb.b - b) * dy) 
#endif
{
#ifdef ILL_SSE_IN_API
	m_xyuv = sa.m_xyuv;
	m_rgbz = sa.m_rgbz;
	m_dxyuv = _mm_sub_ps(sb.m_xyuv, sa.m_xyuv);
	m_drgbz = _mm_sub_ps(sb.m_rgbz, sa.m_rgbz);
	f4 mask = _mm_div_ps(_mm_shuffle_ps(m_dxyuv, m_dxyuv, 0x55), _mm_load_ps1(&step)); //_MM_SHUFFLE(1, 1, 1, 1)
	m_dxyuv = _mm_div_ps(m_dxyuv, mask);
	m_drgbz = _mm_div_ps(m_drgbz, mask);
#endif
}

