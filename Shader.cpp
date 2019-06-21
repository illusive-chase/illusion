#include "Shader.h"
#include "STexture.h"

fl::geom::Shader::Shader(DWORD * write_src, MapTrait* map_trait_src, int width, void* obj, const Texture & t,
	const int offset) :
	src(reinterpret_cast<const BYTE*>(t.src)), src_wid(t.width), src_size(t.width * t.height),
	offset(offset), step(1 << offset), write_src(write_src),
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
	if (map_trait->z_depth < shadee.z) {
		int index = shadee.getU() + src_wid * shadee.getV();
		if (index < 0 || index >= src_size) return;
		*write = ((DWORD*)src)[index];
		map_trait->r = shadee.r;
		map_trait->g = shadee.g;
		map_trait->b = shadee.b;
		map_trait->z_depth = shadee.z;
		map_trait->object = obj;
	}
}

void fl::geom::Shader::shade_follow(float z, int id) {
	if (map_trait->z_depth < z) {
		*write = write[-id];
		*map_trait = map_trait[-id];
		map_trait->z_depth = z;
	}
}
