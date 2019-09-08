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
/// @date 2019/7/2
////////////////////////////////////////////

#include "stdafx.h"


#define import_all
#include "top_element/SImport.h"
#undef import_all

extern Stage stage;


Stage3D wd;
ModelLoader ml;
Roamer roamer;
ImageLoader ld;
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

#include "top_element\SPattern.h"

// Initialize
void System::Setup() {
	
	/*ImageLoader ld = new ImageLoaderImpl();
	stage.addRecycleListener(ld);
	ld->load(L"src\\sky.bmp");
	Texture tx(ld->src(0), ld->width(0), ld->height(0), 3.3f, 0, 0);

	stage.addConsole();
	stage.addChild(wd = new Stage3DImpl(0, 0, 1024, 768, 0, 2000, 12, Stage3DImpl::MODE_MLAA, 1, new SkyBoxImpl(tx, 2000)));
	wd->addLight(new DirectionalLight3DImpl(Vector3D(1, -1, 1), Vector3D(0.6f, 0.6f, 0.6f)));
	wd->addLight(new Light3DImpl(Vector3D(0.4f, 0.4f, 0.4f)));
	wd->setCamera(Vector3D(0, 0, 0), Vector3D(0, 0, -70));
	wd->addObject(new SQuadr3D(Vector3D(0, -2000, 0), Vector3D(2000, 0, 0), Vector3D(0, 0, -2000), Color::GREEN));

	stage.addRecycleListener(ml = new ModelLoaderImpl());
	ml->loadMMD(L"C:\\Users\\Ҷ��\\Desktop\\3dmax\\model", L"mmd.obj", 12, true);
	wd->addObject(new SObject3DImpl(*(ml->models[0])));
	
	//Texture suf(Color::RED);
	//PObject3D partice = new PObject3DImpl(10, Vector3D()); partice->addForce(PFGravity(0.098f)); stage.addRecycleListener(partice);
	//SObject3D cube = new Cube3D(Vector3D(0, 0, 0), suf); cube->addPObject(partice); wd->addObjectWithPosition(cube);

	stage.addRecycleListener(new RoamerImpl(wd));*/
	


#if 0
	stage.addConsole();

	ld = MakeImageLoader();
	ld->load(L"ass\\sky.bmp");
	Texture tx(ld->src(0), ld->width(0), ld->height(0), 3.3f, 0, 0);
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
	//roamer = MakeRoamer(wd);

	addBall(10.0f, Vector3D(8, 100, -60));
	addBall(1.0f, Vector3D(0, -10, -60));
	InitWindow();
#endif

	stage.addConsole();

	int yy = 2019, mm = 8, dd = 21;
	SPattern<3> sp(L"YY", L"MM", L"DD");
	sp.read(yy, mm, dd);
	std::wcout << sp.to_string(L"YY/MM/DD") << std::endl;
	sp.format(L"YY-MM-DD", L"2019-8-21");
	sp.write(yy, mm, dd);
	std::wcout << yy << L" " << mm << L" " << dd << std::endl;
}


 