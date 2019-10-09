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
#include "../display/SColor.h"

namespace fl {
	namespace geom {
		class Texture {
		public:
			Bitmap bmp;
			int width, height;
			scalar Ka, Kd, Ks; // coefficient of ambient light, diffuse reflection and specular reflection 

			Texture(Bitmap bmp, scalar Ka = 0.8f, scalar Kd = 0.8f, scalar Ks = 0.1f)
				:bmp(bmp), Ka(Ka), Kd(Kd), Ks(Ks) {
				width = bmp->width;
				height = bmp->height;
			}

			// This constructor can use Color::XXX to form a Texture.
			// It even enables Color::XXX to be implicitly converted to class Texture.
			Texture(DWORD pure_color, scalar Ka = 0.8f, scalar Kd = 0.8f, scalar Ks = 0.1f) 
				:bmp(MakeBitmap(1, 1, new DWORD[1])), Ka(Ka), Kd(Kd), Ks(Ks)
			{
				bmp->src[0] = pure_color;
				width = bmp->width;
				height = bmp->height;
			}

			Texture(const Texture& copy) : bmp(copy.bmp), Ka(copy.Ka), Kd(copy.Kd), Ks(copy.Ks) {
				width = bmp->width;
				height = bmp->height;
			}

			Texture(Texture&& copy) noexcept :bmp(std::move(copy.bmp)), width(copy.width), height(copy.height), Ka(copy.Ka), Kd(copy.Kd), Ks(copy.Ks) {}

			Texture& operator =(const Texture& copy) = delete;
			Texture& operator =(Texture&& copy) = delete;

			~Texture() {}
		};
	}
}