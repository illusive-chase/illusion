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
#include "SObject3D.h"

namespace fl {
	namespace geom {

		class SkyBoxImpl :public SObject3DImpl {
		private:
			DWORD* top_src; // It saves the automatically generated top texture.

		public:
			const int size; // a half of side length

			ILL_INLINE void framing() override {}

			// It uses 'tex' as a texture for the four sides
			// and automatically generates the top texture by vertex interpolation.
			SkyBoxImpl(const Texture& tex, int size);

			~SkyBoxImpl() { if (top_src) delete[] top_src; }
		};

		using SkyBox = sptr<SkyBoxImpl>;
		ILL_INLINE SkyBox MakeSkyBox(const Texture& tex, int size) {
			return SkyBox(new SkyBoxImpl(tex, size));
		}
	}
}

