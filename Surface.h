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
#include "STexture.h"

// This part should be improved by adding SSE/SSE2 optimization.

namespace fl {
	namespace geom {
		class Surface3D {
		public:
			int link_index[3];

			Texture texture;
			int uv[3];

			Surface3D(int pa, int pb, int pc, const  Texture& texture, int uv0, int uv1, int uv2) :texture(texture) {
				link_index[0] = pa;
				link_index[1] = pb;
				link_index[2] = pc;
				uv[0] = uv0;
				uv[1] = uv1;
				uv[2] = uv2;
			}
		};
	}
}