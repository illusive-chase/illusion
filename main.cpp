////////////////////////////////////////////
/// Illusion Library
/// GUI for Windows
///
///
/// @file main.cpp
/// @brief 外部接口文件，核心：Setup函数
/// @author illusion
/// @version 1.5
/// @date 2019/6/5
////////////////////////////////////////////

#include "stdafx.h"
#include "illusion.h"

/// @brief 定义宏用于导入
/// @see 头文件 SImport.h
///
#define import_all
#include "SImport.h"

extern Stage stage;


Stage3D* wd;
ModelLoader* ml;


/// @brief 外部接口函数，调用于窗口初始化
/// @param void
/// @return void
///
void fl::Setup() {

	ImageLoader* ld = new ImageLoader();
	stage.addRecycleListener(ld);
	ld->load(L"src\\sky.bmp");
	Texture tx(ld->src(0), ld->width(0), ld->height(0), 3.3, 0, 0);

	stage.addConsole();
	stage.addChild(wd = new Stage3D(0, 0, 1024, 768, 0, 2000, 12, Stage3D::MODE_MLAA, 1, new SkyBox(tx, 2000)));
	wd->addLight(new DirectionalLight3D(Vector3D(1, -1, 1), Vector3D(0.6, 0.6, 0.6)));
	wd->addLight(new Light3D(Vector3D(0.4, 0.4, 0.4)));
	wd->setCamera(Vector3D(0, 0, 0), Vector3D(0, 0, -70));
	wd->addObject(new SQuadr3D(Vector3D(0, -2000, 0), Vector3D(2000, 0, 0), Vector3D(0, 0, -2000), Color::GREEN));

	stage.addRecycleListener(ml = new ModelLoader());
	ml->loadMMD(L"C:\\Users\\叶开\\Desktop\\3dmax\\model", L"mmd.obj", 12, true);
	wd->addObject(new SObject3D(*(ml->models[0])));

	//Texture suf(Color::RED);
	//PObject3D* partice = new PObject3D(10, Vector3D()); partice->addForce(PFGravity(0.098)); stage.addRecycleListener(partice);
	//SObject3D* cube = new Cube3D(Vector3D(0, 0, 0), suf); cube->addPObject(partice); wd->addObjectWithPosition(cube);

	stage.addRecycleListener(new Roamer(wd));
	InitWindow();
}