#include "Shader.h"
#include "STexture.h"

fl::geom::Shader::Shader(DWORD * write_src, MapTrait* map_trait_src, int width, void* obj, const Texture & t,
	const int offset) :
	src(reinterpret_cast<const BYTE*>(t.src)), src_wid(t.width), offset(offset), step(1 << offset), write_src(write_src),
	map_trait_src(map_trait_src), width(width), obj(obj)
{
	write = write_src;
	map_trait = map_trait_src;
}

void fl::geom::Shader::moveTo(int x, int y, int channel) {
	int pos = ((x + y * width) << offset) | channel;
	write = write_src + pos;
	map_trait = map_trait_src + pos;
}

void fl::geom::Shader::move(int i) {
	write += i << offset;
	map_trait += i << offset;
}

void fl::geom::Shader::move() {
	write += step;
	map_trait += step;
}

void fl::geom::Shader::shade(const LerpX& shadee, DWORD mask) {
	const BYTE* pick = src + ((shadee.getU() + src_wid * shadee.getV()) << 2);
	if (map_trait->z_depth < shadee.z) {
#ifdef SSE
		const __m128i zero = _mm_setzero_si128();
		__m128 tmp = _mm_set_ps(0, (float)shadee.r, (float)shadee.g, (float)shadee.b);
		tmp = _mm_div_ps(tmp, _mm_set1_ps((float)shadee.z));
		__m128i tmpi = _mm_packus_epi16(_mm_packs_epi32(_mm_cvtps_epi32(_mm_mul_ps(_mm_cvtepi32_ps(_mm_set_epi32(0, pick[2], pick[1], pick[0])), tmp)),
			zero), zero);
		*write = (~_mm_cvtsi128_si32(tmpi)) & mask;
#else
		*write = ((~RGB3D(MIX(pick[2], shadee.r / shadee.z), MIX(pick[1], shadee.g / shadee.z), MIX(pick[0], shadee.b / shadee.z))) & mask);
#endif
		map_trait->z_depth = shadee.z;
		map_trait->object = obj;
	}
}

void fl::geom::Shader::shade_follow(float z, int id) {
	if (map_trait->z_depth < z) {
		*write = write[-id];
		map_trait->z_depth = z;
		map_trait->object = obj;
	}
}
