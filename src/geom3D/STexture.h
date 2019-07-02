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
#include "../top_element/Scalar.h"

namespace fl {
	namespace geom {
		class Texture {
		public:
			DWORD* src;
			int width, height;
			scalar Ka, Kd, Ks; // coefficient of ambient light, diffuse reflection and specular reflection 

			Texture(DWORD* src, int width, int height, scalar Ka = 0.8f, scalar Kd = 0.8f, scalar Ks = 0.1f)
				:src(src), Ka(Ka), Kd(Kd), Ks(Ks), width(width), height(height) {
			}

			// This constructor can use Color::XXX to form a Texture.
			// It even enables Color::XXX to be implicitly converted to class Texture.
			Texture(DWORD& pure_color, scalar Ka = 0.8f, scalar Kd = 0.8f, scalar Ks = 0.1f) :src(&pure_color), Ka(Ka), Kd(Kd), Ks(Ks), width(1), height(1) {}
		};
	}
}