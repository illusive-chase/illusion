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
#define import_all
#include "../src/top_element/SImport.h"
#undef import_all
#include <cmath>
#include <iomanip>
using namespace std;


struct Point3D { double x, y, z; };

struct Point2D { double x, y; };


//�㷨�࣬������ֱ任�㷨
class Algorithm {

public:
	//ƽ��ͶӰ
	static void ParallelProjection(Point3D* point3D, Point2D* point2D, int length);

	//͸��ͶӰ
	static void PerspectiveProjection(Point3D* point3D, Point2D* point2D, int length);



	//͸��ͶӰ��͸�ӵ� �� ƽ��ͶӰ�ķ�������
	static Point3D pers;

	//ͶӰ�ӿں���������ά������Ӧ�Ķ�άͶӰ����
	static void(*Project)(Point3D* point3D, Point2D* point2D, int length);

	//������������е���y����ת���Ƕ�Ϊ����ʾ��ʱ��
	template<int length>
	static void RotateAroundYaxis(Point3D (&points)[length], double degree);

	//����һ������߷ֱ�Ϊabc�ġ���y����ת��degree�Ƕȵĳ����壬��xyz����������Ľؾ�
	static Point3D GetCut(double a, double b, double c, double degree);

};

void (*Algorithm::Project)(Point3D* point3D, Point2D* point2D, int length) = Algorithm::PerspectiveProjection;
Point3D Algorithm::pers = { 300,300,300 };


//�滭�࣬��������߶�
class PainterImpl {
public:
	int width, height;
	DWORD* src;
	double* depth;
	PainterImpl(int width, int height, DWORD* src) :width(width), height(height), src(src), depth(new double[width * height]) {}


	//Bresenham�㷨����һ����Ļ�ϵ�ֱ�ߣ�������ÿ�����ص����ֵ������������ص���ɫ�Ƿ񱻸���
	void line(Point2D a, double az, Point2D b, double bz, int color) {
		double dz = bz - az;

		//��ֱ�߲ü���ʹ���ڻ����ķ�Χ��
		{
			double dx = b.x - a.x;
			double dy = b.y - a.y;
			a.x += width / 2;
			a.y += height / 2;
			b.x += width / 2;
			b.y += height / 2;
			if (dx) {
				if (a.x < 0) az += (-a.x) * dz / dx, a.y += (-a.x) * dy / dx, a.x = 0;
				else if (a.x >= width - 1) az -= (a.x - width + 2) * dz / dx, a.y -= (a.x - width + 2) * dy / dx, a.x = width - 2;
				if (b.x < 0) bz += (-b.x) * dz / dx, b.y += (-b.x) * dy / dx, b.x = 0;
				else if (b.x >= width - 1) bz -= (b.x - width + 2) * dz / dx, b.y -= (b.x - width + 2) * dy / dx, b.x = width - 2;
			}
			if (dy) {
				if (a.y < 0) az += (-a.y) * dz / dy, a.x += (-a.y) * dx / dy, a.y = 0;
				else if (a.y >= height - 1) az -= (a.y - height + 2) * dz / dy, a.x -= (a.y - height + 2) * dx / dy, a.y = height - 2;
				if (b.y < 0) bz += (-b.y) * dz / dy, b.x += (-b.y) * dx / dy, b.y = 0;
				else if (b.y >= height - 1) bz -= (b.y - height + 2) * dz / dy, b.x -= (b.y - height + 2) * dx / dy, b.y = height - 2;
			}
			if (a.x >= width - 1 || a.y >= height - 1 || a.x < 0 || a.y < 0 || b.x < 0 || b.y < 0 ||
				b.x >= width - 1 || b.y >= height - 1) return;
		}

		//Bresenham�㷨
		int x1 = (int)round(b.x), x0 = (int)round(a.x);
		int y1 = (int)round(b.y), y0 = (int)round(a.y);
		int dx = (int)round(b.x - a.x);
		int dy = (int)round(b.y - a.y);

		int step_x = 1, step_y = 1, swap_flag = 0;
		if (dx < 0) dx = -dx, step_x = -1;
		if (dy < 0) dy = -dy, step_y = -1;

		if (dy > dx) std::swap(dx, dy), swap_flag = ~0;
		int f = (dy * 2) - dx;
		dz /= dx;
		for (int i = 0; i <= dx; ++i) {
			if (az > depth[x0 + y0 * width]) src[x0 + y0 * width] = color, depth[x0 + y0 * width] = az;
			if (az > depth[x0 + y0 * width + 1]) src[x0 + y0 * width + 1] = color, depth[x0 + y0 * width + 1] = az;
			if (az > depth[x0 + y0 * width + width]) src[x0 + y0 * width + width] = color, depth[x0 + y0 * width + width] = az;
			if (az > depth[x0 + y0 * width + width + 1]) src[x0 + y0 * width + width + 1] = color, depth[x0 + y0 * width + width + 1] = az;
			if (f >= 0) {
				x0 += step_x & swap_flag;
				y0 += step_y & ~swap_flag;
				f -= dx * 2;
			}
			f += dy * 2;
			x0 += step_x & ~swap_flag;
			y0 += step_y & swap_flag;
			az += dz;
		}
	}
};

using Painter = sptr<PainterImpl>;



//�������࣬���𳤷���Ľ�ģ��ͶӰ
class Cuboid {

	//���ݽṹ���߶�
	struct Line {
		int a, b;   //�����յ������
		DWORD color;    //��ɫ
	};

public:

	double degree; //ÿ����ת�ĽǶȣ�������ʾ��ʱ�룬������ʾ˳ʱ�룬0��ʾ����ת

	Point3D point3D[8];  //��ά������飬��ʾ�������8������

//8�����λ�ã�
/*            4-------5
 *           /|      /|
 *          0-------1 |       y
 *          | 7-----|-6       |
 *          |/      |/        O----x
 *          3-------2        /
 *                          z
 */

	Point2D point2D[8];  //��ά������飬�ֱ��ʾ�������8�������Ӧ��ͶӰ��
	Line lines[12];      //������������飬�ֱ��ʾ�������12������߶�

	double a, b, c;      //2a��2b��2c��ʾ������ĳ����
	double d;               //d��ʾ�������Ѿ���ת�ĽǶ�

	Painter painter;

	//��������߷ֱ�Ϊ2a��2b��2c�ĳ�����
	Cuboid(double a, double b, double c, Painter painter) : degree(0), point3D(), point2D(), a(a), b(b), c(c), d(0), painter(painter) {
		//����8������
		point3D[0] = { -a, b, c };
		point3D[1] = { a, b, c };
		point3D[2] = { a, -b, c };
		point3D[3] = { -a, -b, c };
		point3D[4] = { -a, b, -c };
		point3D[5] = { a, b, -c };
		point3D[6] = { a, -b, -c };
		point3D[7] = { -a, -b, -c };

		//����12����
		for (int i = 0; i < 8; i += 4) {
			lines[i] = { i,i + 1,RGB(255,0,0)};
			lines[i + 1] = { i + 1,i + 2,RGB(0,254,0)};
			lines[i + 2] = { i + 2,i + 3,RGB(0,0,255)};
			lines[i + 3] = { i + 3,i,RGB(0,254,0)};
		}
		lines[8] = { 0,4,RGB(255,0,0)};
		lines[9] = { 1,5,RGB(255,0,0)};
		lines[10] = { 2,6,RGB(0,0,255)};
		lines[11] = { 3,7,RGB(0,0,255)};
	}

	//��ͼ����
	void paint() {
		Algorithm::Project(point3D, point2D, 8);
		for (int i = 0; i < 12; ++i) {
			painter->line(point2D[lines[i].a], point3D[lines[i].a].z, point2D[lines[i].b], point3D[lines[i].b].z, lines[i].color);
		}
	}

};

//�������࣬����������Ľ�ģ��ͶӰ
class Axis {
public:

	Point3D point3D[6];  //��ά������飬ǰ�������ʾxyz����������Ľ��㣬���������ʾxyz������ԭ�����̶����ȵĵ�
	Point2D point2D[6];  //��ά������飬�ֱ��ʾ6����ά���ͶӰ��
	Painter painter;

	//ָ�����������ʾ����
	Axis(double length, Painter painter) : point3D(), point2D(), painter(painter) {
		point3D[3] = { length,0,0 }; //x��
		point3D[4] = { 0,length,0 }; //y��
		point3D[5] = { 0,0,length }; //z��
	}

	//��ͼ����
	void paint() {
		Algorithm::Project(point3D, point2D, 6);
		for (int i = 0; i < 3; ++i) {
			// ��ɫ���Ƴ������ڲ���������
			painter->line(Point2D{ 0,0 }, 0.0, point2D[i], point3D[i].z, 0xa0a0a0);
			// ��ɫ���Ƴ������ⲿ��������
			painter->line(point2D[i], point3D[i].z, point2D[3 + i], point3D[3 + i].z, 0x0);
		}
	}

};

//�ͻ����࣬���ڴ����������Ľ����߼�
class Client : public SBitmapImpl {

	template<typename T>
	class ValueSetterImpl : public ShapeImpl {
	public:
		T min_v, max_v;
		wstring name;
		SText name_txt;
		Slider slider;
		SButton btn;
		T& settee;

		//����Ϊ�෴��
		void negativate(MouseEvent e) {
			min_v = -min_v;
			max_v = -max_v;
			settee = -settee;
			name_txt->caption.str(L"");
			name_txt->caption << name;
			name_txt->caption << fixed << setprecision(1) << settee;
		}
		
		//���ݻ���ƫ�ƣ�����ֵ
		void set(SliderImpl::SlideEvent e) { 
			settee = (T)(min_v + e.value * (max_v - min_v));
			name_txt->caption.str(L"");
			name_txt->caption << name;
			name_txt->caption << fixed << setprecision(1) << settee;
		}
		

		ValueSetterImpl(int x, int y, const wstring& name, T min_v, T max_v, bool can_be_neg, T& settee)
			:ShapeImpl(nullptr), min_v(min_v), max_v(max_v), name(name), name_txt(MakeSText(35, 0, L"", SFont(24), this)),
			slider(MakeSlider(115, 3, 100, 18, RGB(35, 72, 137), RGB(125, 170, 250), this)), settee(settee), btn(nullptr)
		{
			this->x = x;
			this->y = y;
			slider->slideEventListener.add(this, SliderImpl::EVENT_SLIDING, &ValueSetterImpl::set);
			set(0.0);
			if (can_be_neg) {
				btn = MakeSButton(0, -6, 30, 30, 0xf0f0f0, RGB(145, 190, 255), L"��", 24, nullptr, nullptr, nullptr, this);
				btn->callback.add(this, WM_LBUTTONDOWN, &ValueSetterImpl::negativate);
			}
		}

		void paint(HDC hdc) override { name_txt->paint(hdc); slider->paint(hdc); if (btn) btn->paint(hdc); }
	};

public:
	Painter painter;   //������
	Axis axis;         //������
	Cuboid cuboid;     //������
	SButton sbtn;      //�ı�ͶӰ���͵İ�ť
	bool perspective;  //�Ƿ�Ϊ͸��ͶӰ
	sptr<ValueSetterImpl<double>> arr[4];  //����
	SText axis_name[3]; //����������

	//�л�ͶӰ����
	void btn_switch(MouseEvent e) {
		auto& tmp = sbtn->caption();
		if (perspective) tmp.str(L"ƽ��ͶӰ  ͶӰ����������"), Algorithm::Project = Algorithm::ParallelProjection;
		else tmp.str(L"͸��ͶӰ  ͸�ӵ����꣺"), Algorithm::Project = Algorithm::PerspectiveProjection;
		for (int i = 0; i < 3; ++i) {
			//���軬���ֵ
			if (perspective) {
				if (arr[i]->min_v < 0) arr[i]->min_v = -1.0, arr[i]->max_v = -6.0;
				else arr[i]->min_v = 1.0, arr[i]->max_v = 6.0;
			} else {
				if (arr[i]->min_v < 0) arr[i]->min_v = -200.0, arr[i]->max_v = -500.0;
				else arr[i]->min_v = 200.0, arr[i]->max_v = 500.0;
			}
			arr[i]->set(arr[i]->slider->degree);
		}
		perspective = !perspective;
	}

	//���������������
	void reset(MouseEvent e) { 
		cuboid = Cuboid(80, 60, 100, painter); 
		arr[3]->set(arr[3]->slider->degree);
	}

	//��ʼ��
	Client(int x, int y, int width, int height)
		:SBitmapImpl(x, y, MakeBitmap(width, height, new DWORD[width * height])),
		painter(new PainterImpl(width, height, keep)),
		axis(180, painter),
		cuboid(80, 60, 100, painter),
		sbtn(MakeSButton(x + width + 10, 50, 235, 40, 0xf0f0f0, RGB(145, 190, 255), L"͸��ͶӰ  ͸�ӵ����꣺", 22)),
		perspective(true)
	{
		sbtn->callback.add(this, WM_LBUTTONDOWN, &Client::btn_switch);
		stage.addChild(sbtn);
		SButton tmp = MakeSButton(x + width + 10, 300, 235, 40, 0xf0f0f0, RGB(145, 190, 255), L"������ת�Ƕ�", 24);
		tmp->callback.add(this, WM_LBUTTONDOWN, &Client::reset);
		stage.addChild(tmp);
		stage.addChild(arr[0] = sptr<ValueSetterImpl<double>>(new ValueSetterImpl<double>(
			x + width + 10, 100, L"X: ", 200.0, 500.0, true, Algorithm::pers.x)));
		stage.addChild(arr[1] = sptr<ValueSetterImpl<double>>(new ValueSetterImpl<double>(
			x + width + 10, 150, L"Y: ", 200.0, 500.0, true, Algorithm::pers.y)));
		stage.addChild(arr[2] = sptr<ValueSetterImpl<double>>(new ValueSetterImpl<double>(
			x + width + 10, 200, L"Z: ", 200.0, 500.0, true, Algorithm::pers.z)));
		stage.addChild(arr[3] = sptr<ValueSetterImpl<double>>(new ValueSetterImpl<double>(
			x + width + 10, 250, L"ת��: ", 0, 50, true, cuboid.degree)));
		axis_name[0] = MakeSText(0, 0, L"x", SFont(20));
		axis_name[1] = MakeSText(0, 0, L" y", SFont(20));
		axis_name[2] = MakeSText(0, 0, L"z", SFont(20));
	}

	//֡������ÿһ֡����ִ��
	void framing() override {
		memset(keep, ~0, (width * height) << 2);
		memset(painter->depth, 0xfe, (width * height) * sizeof(*painter->depth));
		if (cuboid.degree) {
			Algorithm::RotateAroundYaxis(cuboid.point3D, cuboid.degree * MILISECOND_PER_FRAME / 1000);  //��ת
			cuboid.d += cuboid.degree * MILISECOND_PER_FRAME / 1000;
		}
		Point3D p = Algorithm::GetCut(cuboid.a, cuboid.b, cuboid.c, cuboid.d);      //�����������ڳ������ڲ��Ĳ���
		axis.point3D[0].x = p.x;
		axis.point3D[1].y = p.y;
		axis.point3D[2].z = p.z;

		//����������
		axis.paint();
		//���Ƴ�����
		cuboid.paint();
		
		//��������������
		Point3D axis_fix_point3D[3] = { {20,0,0},{0,30,0},{0,0,20} };
		Point2D axis_fix_point2D[3] = {};
		Algorithm::Project(axis_fix_point3D, axis_fix_point2D, 3);
		for (int i = 0; i < 3; ++i) 
			axis_name[i]->x = int(axis_fix_point2D[i].x) + x + painter->width / 2,
			axis_name[i]->y = int(-axis_fix_point2D[i].y) + y + painter->height / 2;
	}

};

void System::Setup() {
	stage.addChild(MakeSRect(0, 0, 800, 600, 1, 0xf0f0f0, PS_NULL));
	sptr<Client> client = sptr<Client>(new Client(30, 30, 500, 500));
	stage.addChild(client);
	stage.addChild(client->axis_name[0]);
	stage.addChild(client->axis_name[1]);
	stage.addChild(client->axis_name[2]);
	stage.showFrameDelay(false);
	InitWindow(L"ͶӰ�ͱ任��ʾ����", 20, 20, 800, 600, WindowStyle(1, 0, 1, 0, 1, 0));
}


//������������е���y����ת���Ƕ�Ϊ����ʾ��ʱ��
template<int length>
void Algorithm::RotateAroundYaxis(Point3D (&points)[length], double degree) {
	//ת���Ƕ�Ϊ���ȣ��������Ǻ���
	double rad = degree * PI / 180.0;
	double cosr = cos(rad);
	double sinr = sin(rad);

	//��y����ת
	for (int i = 0; i < length; ++i) {
		double tempz = points[i].z * cosr - points[i].x * sinr;
		points[i].x = points[i].x * cosr + points[i].z * sinr;
		points[i].z = tempz;
	}
}

//ƽ��ͶӰ
void Algorithm::ParallelProjection(Point3D* point3D, Point2D* point2D, int length) {
	double px = pers.x / pers.z;
	double py = pers.y / pers.z;
	for (int i = 0; i < length; ++i) {
		point2D[i].x = point3D[i].x - px * point3D[i].z;
		point2D[i].y = point3D[i].y - py * point3D[i].z;
	}
}

//͸��ͶӰ
void Algorithm::PerspectiveProjection(Point3D* point3D, Point2D* point2D, int length) {
	for (int i = 0; i < length; ++i) {
		double dz = (pers.z - point3D[i].z);
		point2D[i].x = (point3D[i].x * pers.z - point3D[i].z * pers.x) / dz;
		point2D[i].y = (point3D[i].y * pers.z - point3D[i].z * pers.y) / dz;
	}
}

//����һ������߷ֱ�Ϊabc�ġ���y����ת��degree�Ƕȵĳ����壬��xyz����������Ľؾ�
Point3D Algorithm::GetCut(double a, double b, double c, double degree) {
	//ת���Ƕ�Ϊ���ȣ��������Ǻ���
	double rad = degree * PI / 180.0;
	double cosr = abs(cos(rad));
	double sinr = abs(sin(rad));
	return Point3D{ min(a / cosr,c / sinr),b,min(c / cosr,a / sinr) };
}
