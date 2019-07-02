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
#include "SPointer.h"
#include "../geom3D/SObject3D.h"

namespace fl {
	namespace loader {
		class ImageLoaderImpl {
		public:
			std::vector<DWORD*> content;
			std::vector<BITMAP> info;
			ImageLoaderImpl() {}
			~ImageLoaderImpl() { for (DWORD* p : content) delete p; }
			int load(const wstring& path, int width, int height, int type = IMAGE_BITMAP);
			inline int load(const wstring& path, int type = IMAGE_BITMAP) { return load(path, 0, 0, type); }
			inline int width(int i) { return info[i].bmWidth; }
			inline int height(int i) { return info[i].bmHeight; }
			inline DWORD* src(int i) { return content[i]; }
		};

		using ImageLoader = sptr<ImageLoaderImpl>;
		ILL_INLINE ImageLoader MakeImageLoader() {
			return ImageLoader(new ImageLoaderImpl());
		}

		// Class ModelLoaderImpl only have one interface now. It can only load MMD model now.
		class ModelLoaderImpl {
		private:
			ImageLoader uv_loader;
		public:
			std::vector<geom::SObject3D> models;
			ModelLoaderImpl() {}
			~ModelLoaderImpl() {}
			int loadMMD(const wstring& dir, const wstring& name, scalar scale = 12, bool leftTopOrigin = false);
		};

		using ModelLoader = sptr<ModelLoaderImpl>;
		ILL_INLINE ModelLoader MakeModelLoader() {
			return ModelLoader(new ModelLoaderImpl());
		}
	}
}