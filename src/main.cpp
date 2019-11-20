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
////////////////////////////////////////////
/// Illusion Library
/// GUI for Windows
///
///
/// @file main.cpp
/// @author illusion
/// @version 0.1
/// @date 2019/10/9
////////////////////////////////////////////

#if 1

#define import_all
#include "top_element/SImport.h"
#undef import_all



extern Stage stage;


Stage3D wd;
Roamer roamer;
Phase ph;
PObject3D ptrace, ptrace2;


void trace(FrameEvent e) {
	printf("mi x: %f, y: %f, z: %f\nmx x: %f, y: %f, z: %f\n", ptrace->aabb.mi.x, ptrace->aabb.mi.y, ptrace->aabb.mi.z,
		ptrace->aabb.mx.x, ptrace->aabb.mx.y, ptrace->aabb.mx.z);
	printf("mi x: %f, y: %f, z: %f\nmx x: %f, y: %f, z: %f\n", ptrace2->aabb.mi.x, ptrace2->aabb.mi.y, ptrace2->aabb.mi.z,
		ptrace2->aabb.mx.x, ptrace2->aabb.mx.y, ptrace2->aabb.mx.z);
}

void addBall(float w, const Vector3D& pos, const Vector3D& vel0 = Vector3D()) {
	PSphere psp = MakePSphere(w, pos, 10, 1.0f);
	psp->acc = vel0;
	ph->addObject(psp, true);
	SObject3D sp = MakeSphere3D(Vector3D(), Color::RED, 10, 10, 10);
	wd->addObject((sp->addPObject(psp), sp));
}

#define MMD

float randf(float c = 0.0f, float d = 0.5f) {
	return (rand() % 101) * 0.02f * d + c - d;
}

struct ray {
	Vector3D p, d;
	Vector3D pointer(scalar t) const { return p + d * t; }
};

struct sphere {
	Vector3D p;
	scalar r;
	Vector3D c;
	ray(sphere::* reflect_ray) (const Vector3D& lit);
	scalar hit(const ray& ra) {
		//(ra.p+t*ra.d-p).mod2()=r*r
		//(t*ra.d+oc).mod2()=r^2
		//ra.d.mod2()*t^2+2oc*ra.d*t+oc.mod2()-r^2;
		Vector3D oc = ra.p - p; //(0,249t-100,-20t+100)
		scalar a = ra.d.mod2();
		scalar b = 2 * (oc * ra.d);
		scalar c = oc.mod2() - r * r;
		scalar d = b * b - a * c * 4;
		return d < 0 ? scalar(-1) : ((-b - illSqrt(d)) / (a * 2));
	}
	Vector3D reflect_color(const Vector3D& lit) {
		return Vector3D(lit.x * c.x, lit.y * c.y, lit.z * c.z);
	}
	ray reflect_ray_normal(const Vector3D& rp) {
		ray ra;
		ra.p = rp;
		Vector3D n = ra.p - p;
		n.normalize();
		Vector3D rd(randf(), randf(), randf());
		if (rd == Vector3D()) rd = Vector3D(1, 1, 1);
		rd.normalize();
		rd += n;
		if (rd == Vector3D()) rd = Vector3D(1, 1, 1);
		ra.d = rd;
		return ra;
	}
	template<int k>
	ray reflect_ray_metal(const Vector3D& rp) {
		ray ra;
		ra.p = rp;
		Vector3D n = ra.p - p;
		n.normalize();
		ra.d = rp - n * ((rp * n) * 2);
		ra.d.normalize();
		Vector3D rd(randf(), randf(), randf());
		if (rd == Vector3D()) rd = Vector3D(1, 1, 1);
		rd.normalize();
		ra.d = ra.d * k + rd;
		ra.d.normalize();
		return ra;
	}
	sphere(const Vector3D& p, scalar r, DWORD color, bool metal) :p(p), r(r), 
		c(Vector3D(int(color >> 16) & 0xff, int(color >> 8) & 0xff, (int)color & 0xff)* (1.0f / 255.9f)),
		reflect_ray(metal ? &sphere::reflect_ray_metal<7> : &sphere::reflect_ray_normal) {}
	sphere(const Vector3D& p, scalar r, const Vector3D& color, bool metal) :p(p), r(r),
		c(color), reflect_ray(metal ? &sphere::reflect_ray_metal<7> : &sphere::reflect_ray_normal) {}
};

Vector3D get_color(std::list<sphere>& ls, const ray& r, int depth = 50) {
	if (!depth) return Vector3D();
	scalar min_rr = INF;
	sphere* min_s = nullptr;
	for (sphere& s : ls) {
		scalar rr;
		if ((rr = s.hit(r)) > 0 && min_rr > rr) min_rr = rr, min_s = &s;
	}
	//reflection
	if (min_s) return min_s->reflect_color(get_color(ls, (min_s->*min_s->reflect_ray)(r.pointer(min_rr)), depth - 1));
	Vector3D tmp = r.d;
	tmp.normalize();
	scalar t = 0.5f * (tmp.y + 1.0f);
	return Vector3D(0.5f, 0.7f, 1.0f) * t + Vector3D(1, 1, 1) * (1.0f - t); //env
}

// Initialize
void System::Setup() {

#ifdef RAY

	

	InitWindow(L"Ray Tracing", 20, 20, 500, 500);
	SBitmap sbmp = MakeSBitmap(0, 0, MakeBitmap(500, 500));
	DWORD* src = sbmp->content();
	stage.addChild(sbmp);
	
	std::list<sphere> ls;
	ls.push_back(sphere(Vector3D(0, 0, -250), 125, Vector3D(0.8f, 0.3f, 0.3f), 0));
	ls.push_back(sphere(Vector3D(-250, 0, -250), 125, Vector3D(0.8f, 0.8f, 0.8f), 1));
	ls.push_back(sphere(Vector3D(250, 0, -250), 125, Vector3D(0.8f, 0.6f, 0.2f), 1));
	ls.push_back(sphere(Vector3D(0, -250125, -250), 250000, Vector3D(0.8f, 0.8f, 0.0f), 0));
	for (int i = 0; i < 500; ++i) {
		for (int j = 0; j < 500; ++j) {
			Vector3D cl;
			int nk = 100;
			for (int k = 0; k < nk; ++k) {
				ray r;
				r.p = Vector3D(0, 0, 0);
				r.d = Vector3D(-250.0f + i + randf(0.5), -250.0f + j + randf(0.5), -150.0f);
				r.d.normalize();
				cl += get_color(ls, r);
			}
			cl /= float(nk);
			cl.x = illSqrt(cl.x), cl.y = illSqrt(cl.y), cl.z = illSqrt(cl.z);
			src[i + j * 500] = RGB3D(int(cl.x * 255.9f), int(cl.y * 255.9f), int(cl.z * 255.9f));
		}
	}



#endif


#ifdef MMD
	ImageIO ld = MakeImageIO();
	ld->load(L"ass\\sky.bmp");
	Texture tx(ld->get(0), 3.3f, 0, 0);

	wd = MakeStage3D(0, 0, 1024, 768, 0, 2000, 12, Stage3DImpl::MODE_MLAA, 2, MakeSkyBox(tx, 2000));
	stage.addChild(wd);
	wd->addLight(MakeDirectionalLight3D(Vector3D(1, -1, 1), Vector3D(0.6f, 0.6f, 0.6f)));
	wd->addLight(MakeLight3D(Vector3D(0.4f, 0.4f, 0.4f)));
	wd->setCamera(Vector3D(0, 0, 0), Vector3D(0, 0, -70));

	wd->addObject(MakeModelIO()->loadMMD(L"C:\\Users\\illusion\\Desktop\\3dmax\\model", L"mmd.obj", 12, true));
	roamer = MakeRoamer(wd);
	stage.addConsole();
	InitWindow();
#endif
#ifdef PHY
	stage.addConsole();

	ImageIO ld = MakeImageIO();
	ld->load(L"ass\\sky.bmp");
	Texture tx(ld->get(0), 3.3f, 0, 0);
	ph = MakePhase(.98f, 0.1f);
	PQuad pqd = MakePQuad(Vector3D(0, -20, -60), Vector3D(200, 0, 0), Vector3D(0, 0, 200), 0.8f);
	ph->addObject(pqd, true);
	stage.frameEventListener.add(ph.raw(), WM_FRAME, &PhaseImpl::framing);
	stage.addChild(wd = MakeStage3D(0, 0, 1024, 768, 0, 2000, 12, Stage3DImpl::MODE_MLAA, 1, MakeSkyBox(tx, 2000)));
	wd->addLight(MakeDirectionalLight3D(Vector3D(1, -1, 1), Vector3D(0.6f, 0.6f, 0.6f)));
	wd->addLight(MakeLight3D(Vector3D(0.4f, 0.4f, 0.4f)));
	wd->setCamera(Vector3D(0, 0, 0), Vector3D(0, 0, -70));
	SObject3D sq = MakeSQuadr3D(Vector3D(), Vector3D(0, 0, 200), Vector3D(200, 0, 0), Color::GREEN);
	wd->addObject((sq->addPObject(pqd), sq));
	roamer = MakeRoamer(wd);

	addBall(10.0f, Vector3D(8, 100, -60));
	addBall(1.0f, Vector3D(0, -10, -60));
	InitWindow();
#endif

}


#endif