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

#define MMD

extern Stage stage;


#if defined(PHY) || defined(MMD)
Stage3D wd;
Roamer roamer;
#endif
#ifdef PHY
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
#endif
#ifdef RAC
Camera cam(200.0f, 0.0f, 0.0f);
World w;
#endif

// Initialize
void System::Setup() {
	
	

	
#ifdef RAC
	InitWindow(L"Ray Tracing", 20, 20, 500, 500);
	
	stage.addChild(w.sbmp);
	w.add(RRect(Vector3D(-250, 0, -450), 250, &RRect::YZ, Material(Vector3D(0.4f, 0.4f, 1.0f), &Material::reflect_normal)));
	w.add(RRect(Vector3D(250, 0, -450), 250, &RRect::YZ, Material(Vector3D(1.0f, 0.4f, 0.4f), &Material::reflect_normal)));
	w.add(RRect(Vector3D(0, 0, -700), 250, &RRect::XY, Material(Vector3D(0.4f, 1.0f, 0.4f), &Material::reflect_normal)));
	w.add(RRect(Vector3D(0, -250, -450), 250, &RRect::ZX, Material(Vector3D(0.8f, 0.8f, 0.8f), &Material::reflect_normal)));
	w.add(RRect(Vector3D(0, 250, -450), 250, &RRect::ZX, Material(Vector3D(0.8f, 0.8f, 0.8f), &Material::reflect_normal)));
	w.add(RRect(Vector3D(0, 248, -450), 50, &RRect::ZX, Material(Vector3D(15, 15, 15), &Material::reflect_light)));
	w.add(RSphere(Vector3D(0, -150, -450), 100.0f, Material(Vector3D(1, 1, 1), &Material::reflect_transparent<700>)));
	w.add(RSphere(Vector3D(0, -155, -450), -95.0f, Material(Vector3D(1, 1, 1), &Material::reflect_transparent<700>)));
	cam.setCamera(Vector3D(), Vector3D(0, 0, -1));
	w.render(cam);
#endif

#ifdef MMD
	ImageIO ld = MakeImageIO();
	ld->load(L"ass\\sky.bmp");
	Texture tx(ld->get(0), 3.3f, 0, 0);

	

	wd = MakeStage3D(0, 0, 1024, 768, 0, 2000, 12, Stage3DImpl::MODE_NOSAMPLING, 2, MakeSkyBox(tx, 2000));
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