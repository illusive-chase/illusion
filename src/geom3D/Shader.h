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
#include "SGeomMath.h"
#include "STexture.h"

namespace fl {
	namespace geom {


		// Class MapTrait provides rendering information for class Shader.
		// Each MapTrait object corresponds to one sample pixel.
		// There is a MapTrait array saved in the class SwapChain. See class Stage3DImpl::SwapChain in Stage3DImpl.h.
		// Class Shader writes color information and depth information into this struct and 
		// then the color will be painted on the screen later.
		// See class Shader in Shader.h or function Stage3DImpl::render in Stage3DImpl.cpp.
		ILL_ATTRIBUTE_ALIGNED16(struct) MapTrait {
#ifdef ILL_SSE
			union {
				struct { scalar r, g, b, z_depth; }; // In fact, 'z_depth' is the reciprocal of the actual z depth.
				struct { f4 m_rgbz; };
			};
#else
			scalar z_depth, r, g, b;
#endif
			// 'z_depth' is set to 0 initially, which means the actual z depth is infinity.
			// It should be guaranteed that z_depth > 0.
			MapTrait() :z_depth(0), r(0), g(0), b(0) {}

			ILL_DECLARE_ALIGNED_ALLOCATOR
		};

		// Class Shader is a helper class to set the depth and color information of the MapTrait.
		// See function Stage3DImpl::drawTriangle and Stage3DImpl::drawTriangleMSAA in Stage3DImpl.cpp.
		class Shader {
		public:
			const BYTE* const src;

			const int src_wid;
			const int src_size;
			const int offset;
			const int step;
			const int width;

			DWORD* const write_src;
			MapTrait* const map_trait_src;

			DWORD* write;
			MapTrait* map_trait;

			Shader(DWORD* write_src, MapTrait* map_trait_src, int width, const Texture& t, const int offset = 0)
				:src(reinterpret_cast<const BYTE*>(t.src)), src_wid(t.width), src_size(t.width * t.height),
				offset(offset), step(1 << offset), write_src(write_src),
				map_trait_src(map_trait_src), width(width)
			{
				write = write_src;
				map_trait = map_trait_src;
			}

			ILL_INLINE void moveTo(int x, int y, int channel) {
				int pos = ((x + y * width) << offset) | channel;
				write = write_src + pos;
				map_trait = map_trait_src + pos;
			}

			ILL_INLINE void move(int i) {
				write += i << offset;
				map_trait += i << offset;
			}

			ILL_INLINE void move() {
				write += step;
				map_trait += step;
			}

			ILL_INLINE void shade(const LerpX& shadee) {
				if (map_trait->z_depth < shadee.z) {
					int index = shadee.getU() + src_wid * shadee.getV();
					*write = reinterpret_cast<const DWORD*>(src)[index];
#ifdef ILL_SSE
					map_trait->m_rgbz = shadee.m_rgbz;
#else
					map_trait->r = shadee.r;
					map_trait->g = shadee.g;
					map_trait->b = shadee.b;
					map_trait->z_depth = shadee.z;
#endif
				}

			}


			// This function is used for MSA. See function Stage3DImpl::drawTriangleMSAA in Stage3DImpl.cpp.
			ILL_INLINE void shade_follow(scalar z, int id) {
				if (map_trait->z_depth < z) {
					*write = write[-id];
					*map_trait = map_trait[-id];
					map_trait->z_depth = z;
				}
			}


		};
	}
}