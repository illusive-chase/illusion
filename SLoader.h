#pragma once
#include "SPointer.h"
#include "SObject3D.h"

namespace fl {
	namespace loader {
		class ImageLoader :public AutoPtr {
		public:
			vector<DWORD*> content;
			vector<BITMAP> info;
			ImageLoader() :AutoPtr() {}
			~ImageLoader() { for (DWORD* p : content) delete p; }
			int load(const wstring& path, int width, int height, int type = IMAGE_BITMAP);
			inline int load(const wstring& path, int type = IMAGE_BITMAP) { return load(path, 0, 0, type); }
			inline int width(int i) { return info[i].bmWidth; }
			inline int height(int i) { return info[i].bmHeight; }
			inline DWORD* src(int i) { return content[i]; }
		};

		class ModelLoader :public AutoPtr {
		private:
			ImageLoader* uv_loader;
		public:
			vector<geom::SObject3D*> models;
			ModelLoader() :AutoPtr() { uv_loader = new ImageLoader(); }
			~ModelLoader() { for (geom::SObject3D* p : models) delete p; delete uv_loader; }
			int loadMMD(const wstring& dir, const wstring& name, float scale = 12.0, bool leftTopOrigin = false);
		};
	}
}