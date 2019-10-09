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

#include "../index/show.hpp"






#if 0

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

#include "top_element\SCommand.h"

using r = Regex<char>;

// Initialize
void System::Setup() {
	stage.addConsole();
	DFA dfa;
	Regex<char> reg = r::MakeRegex("[a|b]*abb");
	dfa.build(reg);
	dfa.print();
	dfa.optimize();

#if 0
	ImageIO ld = MakeImageIO();
	ld->load(L"ass\\sky.bmp");
	Texture tx(ld->get(0), 3.3f, 0, 0);

	wd = MakeStage3D(0, 0, 1024, 768, 0, 2000, 12, Stage3DImpl::MODE_NOSAMPLING, 2, MakeSkyBox(tx, 2000));
	stage.addChild(wd);
	wd->addLight(MakeDirectionalLight3D(Vector3D(1, -1, 1), Vector3D(0.6f, 0.6f, 0.6f)));
	wd->addLight(MakeLight3D(Vector3D(0.4f, 0.4f, 0.4f)));
	wd->setCamera(Vector3D(0, 0, 0), Vector3D(0, 0, -70));

	ModelIO ml = MakeModelIO();
	wd->addObject(ml->loadMMD(L"C:\\Users\\illusion\\Desktop\\3dmax\\model", L"mmd.obj", 12, true));
	roamer = MakeRoamer(wd);
	stage.addConsole();
	InitWindow();
#endif
#if 0
	stage.addConsole();

	ld = MakeImageLoader();
	ld->load<>(L"ass\\sky.bmp");
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

}


#endif