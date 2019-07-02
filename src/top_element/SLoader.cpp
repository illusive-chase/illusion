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
#include "SLoader.h"
#include <map>

int fl::loader::ImageLoaderImpl::load(const wstring& path, int width, int height, int type) {
	HBITMAP hbmp = (HBITMAP)LoadImage(NULL, path.c_str(), type, width, height, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
	if (hbmp == NULL) return -1;
	BITMAP bmp;
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	info.push_back(bmp);
	const int size = bmp.bmWidth * bmp.bmHeight;
	DWORD* save = new DWORD[size];
	GetBitmapBits(hbmp, size << 2, save);
	content.push_back(save);
	DeleteObject(hbmp);
	return (int)content.size() - 1;
}


int fl::loader::ModelLoaderImpl::loadMMD(const wstring& dir, const wstring& name, scalar scale, bool leftTopOrigin) {
	using namespace fl::geom;
	std::wifstream obj(dir + L"\\" + name);
	if (!obj.is_open()) return -1;
	wstring read;
	while (obj >> read && read != L"mtllib");
	obj >> read;
	std::wifstream mtl(dir + L"\\" + read);
	if (!mtl.is_open()) return (obj.close(), -1);

	std::map<wstring, int> uv_index;
	std::map<wstring, int> bitmap_name;
	wstring bitmap;
	try {
		while (1) {
			while (mtl >> read && read != L"newmtl");
			mtl >> read;
			while (mtl >> bitmap && bitmap != L"map_Kd") {
				if (bitmap == L"newmtl") throw std::bad_alloc();
			}
			mtl >> bitmap;
			if (mtl.eof()) break;
			if (bitmap_name.count(bitmap)) uv_index[read] = bitmap_name[bitmap];
			else if (mtl.fail() || (bitmap_name[bitmap] = uv_index[read] = uv_loader->load(dir + L"\\" + bitmap)) == -1) return (obj.close(), mtl.clear(), -1);
		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		obj.close();
		mtl.close();
		return -1;
	}
	mtl.close();
	SObject3D create = MakeSObject3D(Vector3D());
	std::vector<Vector3D> uv_point;
	std::vector<Vector3D> mesh_point;
	std::vector<Vector3D> mesh_normal;
	uv_point.push_back(Vector3D());
	mesh_point.push_back(Vector3D());
	mesh_normal.push_back(Vector3D(1, 0, 0));
	scalar a, b, c;
	int i[3], j[3], k[3];
	int pre_index = -1;
	try {
		while (std::getline(obj, read)) {
			wstringstream wss;
			wss.str(read);
			wss >> read;
			if (read == L"v") {
				wss >> a >> b >> c;
				mesh_point.push_back(Vector3D(a * scale, b * scale, c * scale));
			} else if (read == L"vt") {
				wss >> a >> b;
				if (a < 0.001f) a = 0.001f;
				if (b < 0.001f) b = 0.001f;
				if (a > 0.999f) a = 0.999f;
				if (b > 0.999f) b = 0.999f;
				uv_point.push_back(Vector3D(a, b, scalar(0)));
			} else if (read == L"vn") {
				wss >> a >> b >> c;
				mesh_normal.push_back(Vector3D(a, b, c));
			} else if (read == L"usemtl") {
				wss >> read;
				if (!uv_index.count(read)) {
					throw std::bad_alloc();
				}
				pre_index = uv_index[read];
			} else if (read == L"f") {
				for (int n = 0; n < 3; ++n) {
					(wss >> i[n]).get();
					(wss >> j[n]).get();
					wss >> k[n];
				}
				if (!~pre_index) throw std::bad_alloc();
				const int w = uv_loader->width(pre_index);
				const int h = uv_loader->height(pre_index);
				if (leftTopOrigin)
					create->addSurface(i[0], i[2], i[1], Texture(uv_loader->src(pre_index), w, h),
						UV(int(uv_point[j[0]].x * w), int(uv_point[j[0]].y * h)),
						UV(int(uv_point[j[2]].x * w), int(uv_point[j[2]].y * h)),
						UV(int(uv_point[j[1]].x * w), int(uv_point[j[1]].y * h)));
				else
					create->addSurface(i[0], i[2], i[1], Texture(uv_loader->src(pre_index), w, h),
						UV(int(uv_point[j[0]].x * w), int((1.0 - uv_point[j[0]].y) * h)),
						UV(int(uv_point[j[2]].x * w), int((1.0 - uv_point[j[2]].y) * h)),
						UV(int(uv_point[j[1]].x * w), int((1.0 - uv_point[j[1]].y) * h)));
			}
		}
		if (mesh_normal.size() != mesh_point.size()) throw std::bad_alloc();
		for (int n = 0, len = (int)mesh_normal.size(); n < len; ++n) create->addPoint(mesh_point[n], mesh_normal[n]);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		obj.close();
		return -1;
	}
	models.push_back(create);
	obj.close();
	return (int)models.size() - 1;
}
