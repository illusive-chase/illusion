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
#include "illusion.h"

/// @brief 定义宏用于导入
/// @see 头文件 SImport.h
///
#define import_all
#include "top_element/SImport.h"

extern Stage stage;


Stage3D* wd;
ModelLoader* ml;


// Initialize
void fl::Setup() {
	
	/*ImageLoader* ld = new ImageLoader();
	stage.addRecycleListener(ld);
	ld->load(L"src\\sky.bmp");
	Texture tx(ld->src(0), ld->width(0), ld->height(0), 3.3f, 0, 0);

	stage.addConsole();
	stage.addChild(wd = new Stage3D(0, 0, 1024, 768, 0, 2000, 12, Stage3D::MODE_MLAA, 1, new SkyBox(tx, 2000)));
	wd->addLight(new DirectionalLight3D(Vector3D(1, -1, 1), Vector3D(0.6f, 0.6f, 0.6f)));
	wd->addLight(new Light3D(Vector3D(0.4f, 0.4f, 0.4f)));
	wd->setCamera(Vector3D(0, 0, 0), Vector3D(0, 0, -70));
	wd->addObject(new SQuadr3D(Vector3D(0, -2000, 0), Vector3D(2000, 0, 0), Vector3D(0, 0, -2000), Color::GREEN));

	stage.addRecycleListener(ml = new ModelLoader());
	ml->loadMMD(L"C:\\Users\\叶开\\Desktop\\3dmax\\model", L"mmd.obj", 12, true);
	wd->addObject(new SObject3D(*(ml->models[0])));
	
	//Texture suf(Color::RED);
	//PObject3D* partice = new PObject3D(10, Vector3D()); partice->addForce(PFGravity(0.098f)); stage.addRecycleListener(partice);
	//SObject3D* cube = new Cube3D(Vector3D(0, 0, 0), suf); cube->addPObject(partice); wd->addObjectWithPosition(cube);

	stage.addRecycleListener(new Roamer(wd));*/
	

	//stage.addConsole();

	ImageLoader* ld = new ImageLoader();
	stage.addRecycleListener(ld);
	ld->load(L"src\\sky.bmp");
	Texture tx(ld->src(0), ld->width(0), ld->height(0), 3.3f, 0, 0);

	Phase* ph = new Phase(9.8f, 0.0f);
	PSphere* psp = new PSphere(1.0f, Vector3D(0, 900, -300), 100.0f, 1.0f);
	PSphere* psp2 = new PSphere(0.0f, Vector3D(0, -100, -300), 100.0f, 1.0f);

	ph->addObject(psp, true);
	ph->addObject(psp2, true);

	stage.frameEventListener.add(ph, WM_FRAME, &Phase::framing);
	stage.addRecycleListener(ph);

	stage.addChild(wd = new Stage3D(0, 0, 1024, 768, 0, 2000, 12, Stage3D::MODE_MLAA, 1, new SkyBox(tx, 2000)));
	wd->addLight(new DirectionalLight3D(Vector3D(1, -1, 1), Vector3D(0.6f, 0.6f, 0.6f)));
	wd->addLight(new Light3D(Vector3D(0.4f, 0.4f, 0.4f)));
	wd->setCamera(Vector3D(0, 0, 0), Vector3D(0, 0, -70));
	Sphere3D* sp = new Sphere3D(Vector3D(), Color::RED, 15, 15, 100);
	Sphere3D* sp2 = new Sphere3D(Vector3D(), Color::GREEN, 15, 15, 100);
	wd->addObject(sp->addPObject(psp));
	wd->addObject(sp2->addPObject(psp2));
	stage.addRecycleListener(new Roamer(wd));

	InitWindow();
}