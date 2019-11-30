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
	namespace io {

		enum ImageType { IMAGE_BMP, IMAGE_PNG };

		class ImageIOImpl {
		public:
			std::vector<Bitmap> content;
			std::vector<BITMAP> info;
			ImageIOImpl() {}
			~ImageIOImpl() {}
			template<ImageType = IMAGE_BMP> int load(const wstring & path, int width = 0, int height = 0);
			template<ImageType = IMAGE_BMP> bool save(const wstring & path, DWORD * src, int width, int height);
			ILL_INLINE Bitmap get(int i) { if (~i) return content[i]; return Bitmap(nullptr); }
		};

		using ImageIO = sptr<ImageIOImpl>;
		ILL_INLINE ImageIO MakeImageIO() { return ImageIO(new ImageIOImpl()); }

		// Class ModelIOImpl only have one interface now. It can only load MMD model now.
		class ModelIOImpl {
		public:
			ModelIOImpl() {}
			~ModelIOImpl() {}
			SObject3D loadMMD(const wstring& dir, const wstring& name, scalar scale = 12, bool leftTopOrigin = false);
		};

		using ModelIO = sptr<ModelIOImpl>;
		ILL_INLINE ModelIO MakeModelIO() {
			return ModelIO(new ModelIOImpl());
		}
	}
}

#include <map>
#include <fstream>

template<>	
int fl::io::ImageIOImpl::load<fl::io::IMAGE_BMP>(const wstring& path, int width, int height) {
	HBITMAP hbmp = 
		(HBITMAP)LoadImage(NULL, path.c_str(), IMAGE_BITMAP, width, height, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
	if (hbmp == NULL) return -1;
	BITMAP bmp;
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	info.push_back(bmp);
	int size = bmp.bmWidth * bmp.bmHeight;
	DWORD* save = new DWORD[size];
	GetBitmapBits(hbmp, size << 2, save);
	content.push_back(MakeBitmap(bmp.bmWidth, bmp.bmHeight, save));
	DeleteObject(hbmp);
	return (int)content.size() - 1;
}

template<>
bool fl::io::ImageIOImpl::save<fl::io::IMAGE_PNG>(const wstring& path, DWORD* src, int width, int height) {
	return false;
}

template<>
bool fl::io::ImageIOImpl::save<fl::io::IMAGE_BMP>(const wstring& path, DWORD* src, int width, int height) {
	if (width <= 0 || height <= 0 || !src) return false;
	std::ofstream fw(path, std::ios::binary | std::ios::out);
	if (fw.fail()) return false;
	BITMAPFILEHEADER fh = {};
	fh.bfSize = 54U + ((width * height) << 2);
	fh.bfType = 0x4d42;
	fh.bfOffBits = 54U;
	BITMAPINFOHEADER ih = {};
	ih.biSize = sizeof(BITMAPINFOHEADER);
	ih.biWidth = width;
	ih.biHeight = height;
	ih.biPlanes = 1U;
	ih.biBitCount = 32U;
	fw.write((const char*)&fh, sizeof(BITMAPFILEHEADER));
	fw.write((const char*)&ih, sizeof(BITMAPINFOHEADER));
	DWORD* p = src + (width * height);
	while ((p -= width) >= src) fw.write((const char*)p, unsigned(width << 2));
	fw.close();
	return true;
}


SObject3D fl::io::ModelIOImpl::loadMMD(const wstring& dir, const wstring& name, scalar scale, bool leftTopOrigin) {
	using namespace fl::geom;
	std::wifstream obj(dir + L"\\" + name);
	if (!obj.is_open()) return nullptr;
	wstring read;
	while (obj >> read && read != L"mtllib");
	obj >> read;
	std::wifstream mtl(dir + L"\\" + read);
	if (!mtl.is_open()) return nullptr;

	ImageIO uv_loader = MakeImageIO();
	std::map<wstring, int> uv_index;
	std::map<wstring, int> bitmap_name;
	wstring bitmap;
	while (1) {
		while (mtl >> read && read != L"newmtl");
		mtl >> read;
		while (mtl >> bitmap && bitmap != L"map_Kd") {
			if (bitmap == L"newmtl") return nullptr;
		}
		mtl >> bitmap;
		if (mtl.eof()) break;
		if (bitmap_name.count(bitmap)) uv_index[read] = bitmap_name[bitmap];
		else if (mtl.fail() || (bitmap_name[bitmap] = uv_index[read] = uv_loader->load(dir + L"\\" + bitmap)) == -1) return nullptr;
	}
	mtl.close();
	SObject3D create = MakeSObject3D(Vector3D());
	std::vector<Vector3D> uv_point;
	std::vector<Vector3D> mesh_point;
	std::vector<Vector3D> mesh_normal;
	uv_point.push_back(Vector3D());
	mesh_point.push_back(Vector3D());
	mesh_normal.push_back(Vector3D(1, 0, 0));
	Vector3D v;
	int i[3], j[3], k[3];
	Bitmap selected_bmp = nullptr;
	
	while (obj >> read) {
		if (read[0] == L'v') {
			if (read.length() == 1U) {
				obj >> v.x >> v.y >> v.z;
				mesh_point.push_back(v * scale);
			} else if (read[1] == L't') {
				obj >> v.x >> v.y;
				v.x = min(0.999f, max(v.x, 0.001f));
				v.y = min(0.999f, max(v.y, 0.001f));
				v.z = 0;
				uv_point.push_back(v);
			} else if (read[1] == L'n') {
				obj >> v.x >> v.y >> v.z;
				mesh_normal.push_back(v);
			}
		} else if (read == L"usemtl") {
			obj >> read;
			if (!uv_index.count(read)) return nullptr;
			selected_bmp = uv_loader->get(uv_index[read]);
		} else if (read == L"f") {
			for (int n = 0; n < 3; ++n) {
				(obj >> i[n]).get();
				(obj >> j[n]).get();
				obj >> k[n];
			}
			if (!selected_bmp) return nullptr;
			int w = selected_bmp->width;
			int h = selected_bmp->height;
			if (leftTopOrigin)
				create->addSurface(i[0], i[2], i[1], Texture(selected_bmp),
								   UV(int(uv_point[j[0]].x * w), int(uv_point[j[0]].y * h)),
								   UV(int(uv_point[j[2]].x * w), int(uv_point[j[2]].y * h)),
								   UV(int(uv_point[j[1]].x * w), int(uv_point[j[1]].y * h)));
			else
				create->addSurface(i[0], i[2], i[1], Texture(selected_bmp),
								   UV(int(uv_point[j[0]].x * w), int((1 - uv_point[j[0]].y) * h)),
								   UV(int(uv_point[j[2]].x * w), int((1 - uv_point[j[2]].y) * h)),
								   UV(int(uv_point[j[1]].x * w), int((1 - uv_point[j[1]].y) * h)));
		}
		while (obj && obj.get() != L'\n');
	}


	if (mesh_normal.size() != mesh_point.size()) return nullptr;
	for (int n = 0, len = (int)mesh_normal.size(); n < len; ++n) create->addPoint(mesh_point[n], mesh_normal[n]);
	obj.close();
	return create;
}
