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
#include "SkyBox.h"


fl::geom::SkyBox::SkyBox(const Texture& tex, int size) :SObject3D(Vector3D()), size(size >> 1) {
	DWORD uv[10];
	int w = (tex.width - 1) >> 2;
	int h = tex.height - 1;

	top_src = new DWORD[(w + 1) * (w + 1)];
	scalar w_r = scalar(1) / w;
	scalar drdx_l = scalar(int((tex.src[w] >> 16) & 0xFF) - int((tex.src[0] >> 16) & 0xFF)) * w_r;
	scalar dgdx_l = scalar(int((tex.src[w] >> 8) & 0xFF) - int((tex.src[0] >> 8) & 0xFF)) * w_r;
	scalar dbdx_l = scalar(int(tex.src[w] & 0xFF) - int(tex.src[0] & 0xFF)) * w_r;
	scalar drdx_h = scalar(int((tex.src[w << 1] >> 16) & 0xFF) - int((tex.src[3 * w] >> 16) & 0xFF)) * w_r;
	scalar dgdx_h = scalar(int((tex.src[w << 1] >> 8) & 0xFF) - int((tex.src[3 * w] >> 8) & 0xFF)) * w_r;
	scalar dbdx_h = scalar(int(tex.src[w << 1] & 0xFF) - int(tex.src[3 * w] & 0xFF)) * w_r;
	scalar r_l = scalar((tex.src[0] >> 16) & 0xFF);
	scalar g_l = scalar((tex.src[0] >> 8) & 0xFF);
	scalar b_l = scalar(tex.src[0] & 0xFF);
	scalar r_h = scalar((tex.src[3 * w] >> 16) & 0xFF);
	scalar g_h = scalar((tex.src[3 * w] >> 8) & 0xFF);
	scalar b_h = scalar(tex.src[3 * w] & 0xFF);
	int offset = 0;
	for (int x = 0; x <= w; ++x) {
		scalar r = r_l, g = g_l, b = b_l;
		scalar drdy = (r_h - r_l) * w_r;
		scalar dgdy = (g_h - g_l) * w_r;
		scalar dbdy = (b_h - b_l) * w_r;
		for (int y = 0; y <= w; ++y) {
			top_src[offset + y] = RGB3D((BYTE)(int)r, (BYTE)(int)g, (BYTE)(int)b);
			r += drdy;
			g += dgdy;
			b += dbdy;
		}
		offset += w + 1;
		r_l += drdx_l;
		g_l += dgdx_l;
		b_l += dbdx_l;
		r_h += drdx_h;
		g_h += dgdx_h;
		b_h += dbdx_h;
	}
	Texture top_tex(top_src, w + 1, w + 1, tex.Ka, tex.Kd, tex.Ks);


	addPoint(Vector3D(size, size, size)); uv[0] = UV(0, 0);
	addPoint(Vector3D(size, -size, size)); uv[1] = UV(0, h);
	addPoint(Vector3D(-size, -size, size)); uv[2] = UV(w, h);
	addPoint(Vector3D(-size, size, size)); uv[3] = UV(w, 0);
	addPoint(Vector3D(size, size, -size)); uv[4] = UV(3 * w, 0);
	addPoint(Vector3D(size, -size, -size)); uv[5] = UV(3 * w, h);
	addPoint(Vector3D(-size, -size, -size)); uv[6] = UV(2 * w, h);
	addPoint(Vector3D(-size, size, -size)); uv[7] = UV(2 * w, 0);
	uv[8] = UV(4 * w, 0);
	uv[9] = UV(4 * w, h);

	addSurface(0, 3, 2, tex, uv[0], uv[3], uv[2]);
	addSurface(0, 2, 1, tex, uv[0], uv[2], uv[1]);
	addSurface(3, 7, 6, tex, uv[3], uv[7], uv[6]);
	addSurface(3, 6, 2, tex, uv[3], uv[6], uv[2]);
	addSurface(7, 4, 5, tex, uv[7], uv[4], uv[5]);
	addSurface(7, 5, 6, tex, uv[7], uv[5], uv[6]);
	addSurface(4, 0, 1, tex, uv[4], uv[8], uv[9]);
	addSurface(4, 1, 5, tex, uv[4], uv[9], uv[5]);

	addSurface(0, 4, 7, top_tex, UV(0, 0), UV(0, w), UV(w, w));
	addSurface(0, 7, 3, top_tex, UV(0, 0), UV(w, w), UV(w, 0));
}
