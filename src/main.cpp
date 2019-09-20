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


class Grid {
	Sprite grid;
public:
	Sprite nodes;
	int m, n, a;
	Grid() :nodes(nullptr), grid(nullptr), m(0), n(0), a(0) {}

	void init(int m, int n, int a) {
		this->m = m, this->n = n, this->a = a;
		stage.addChild(grid = MakeSprite(0, 0));
		stage.addChild(nodes = MakeSprite(0, 0));
		for (int y = 1; y < m; ++y) grid->addChild(MakeSLine(0, y * a, n * a, y * a, 0, 1, RGB(192, 192, 192)));
		for (int x = 1; x < n; ++x) grid->addChild(MakeSLine(x * a, 0, x * a, m * a, 0, 1, RGB(192, 192, 192)));
	}
	void hide() { if (grid) grid->hide(); }
	void show() { if (grid) grid->show(); }
	void reset(MouseEvent& e) {
		e.x = (e.x + (a >> 1)) / a * a;
		e.y = (e.y + (a >> 1)) / a * a;
	}

} grid;

class NodeImpl :public SRectImpl {
public:
	SText st;
	NodeImpl() :SRectImpl(0, 0, 1, 1, true, Color::FUCHSIA, PS_NULL), st(nullptr) {}
	~NodeImpl() {}

	virtual void paint(HDC hdc) override {
		if (visible) {
			int x0 = x, y0 = y;
			transLocalPosToGlobal(x0, y0);
			DeleteObject(SelectObject(hdc, CreatePen(ps, pwid, pcolor)));
			if (filled) {
				DeleteObject(SelectObject(hdc, CreateSolidBrush(color)));
				Rectangle(hdc, x0, y0, x0 + width, y0 + height);
			} else Rectangle(hdc, x0, y0, x0 + width, y0 + height);
			if (st) st->paint(hdc);
		}
	}

	static void show_text(sptr<NodeImpl>& node, const wstring& txt) {
		if (txt.empty()) node->st = nullptr;
		else node->st = MakeSText(grid.a, grid.a, txt, SFont(20), node);
	}
};

using Node = sptr<NodeImpl>;
Node MakeNode() { return Node(new NodeImpl()); }

class SelectNode :public ToolBase {
private:
	Node sl;
	int rx, ry;
	int level;
	void cancel_select() { sl->ps = PS_NULL; }

	void select(Node r) {
		sl = r;
		sl->ps = PS_INSIDEFRAME;
		sl->pwid = 2;
		sl->pcolor = Color::FUCHSIA;
	}

public:
	SelectNode() :sl(nullptr), rx(0), ry(0), level(0) {}
	void respondM(MouseEvent e) {
		if (e.mk == WM_LDRAG_MK_BEGIN) {
			grid.reset(e);
			if (sl) cancel_select();
			for (Shape sp : grid.nodes->children) {
				if (sp->hitTestPoint(stage.mouseX, stage.mouseY)) {
					select(sp);
					rx = sl->x - e.x;
					ry = sl->y - e.y;
					level = 1;
					break;
				}
			}
		} else if (e.mk == WM_LDRAG_MK_MOVE) {
			if (level == 1) {
				grid.reset(e);
				sl->x = e.x + rx;
				sl->y = e.y + ry;
			}
		} else if (e.mk == WM_LDRAG_MK_END) level = 0;
	}

	void cancel(MouseEvent e) { 
		if (sl) cancel_select(), sl = nullptr;
	}

	void respondK(KeyboardEvent e) {
		if (!sl) return;
		switch (e.keyCode)
		{
		case VK_DELETE: grid.nodes->removeChild(sl); break;
		case VK_UP: sl->y -= grid.a; break;
		case VK_DOWN: sl->y += grid.a; break;
		case VK_LEFT: sl->x -= grid.a; break;
		case VK_RIGHT: sl->x += grid.a; break;
		case 'T':
		{
			stage.showConsole();
			SCommand<> sc;
			wstring tmps;
			int r, g, b;
			for (; sc.read();) {
				if (sc.parse(L"t", tmps)) NodeImpl::show_text(sl, tmps);
				else if (sc.parse(L"c", r, g, b)) sl->color = RGB(r, g, b);
				else break;
			}
			stage.hideConsole();
		}
		default:
			break;
		}
	}

	bool activate() override {
		stage.mouseEventListener.add<SelectNode>(this, WM_LDRAG, &SelectNode::respondM);
		stage.mouseEventListener.add<SelectNode>(this, WM_RBUTTONUP, &SelectNode::cancel);
		stage.keyboardEventListener.add<SelectNode>(this, WM_KEYUP, &SelectNode::respondK);
		return true;
	}

	bool deactivate() override {
		if (level == 1) return false;
		stage.mouseEventListener.remove(this);
		stage.keyboardEventListener.remove(this);
		return true;
	}

};


class CreateNode :public ToolBase {
private:
	int level;
	int cx, cy;
	Node rct;

	static DWORD get_next_color() {
		constexpr int len = 5;
		static DWORD colors[len] = { Color::RED,Color::BLUE,Color::CYAN,Color::GREEN,Color::PURPLE };
		static int t = 0;
		return colors[(t++) % len];
	}

public:
	CreateNode() :level(0), cx(0), cy(0), rct(nullptr) {}
	void respond(MouseEvent e) {
		if (e.mk == WM_LDRAG_MK_BEGIN) {
			grid.reset(e);
			level = 1;
			rct = MakeNode();
			cx = rct->x = e.x;
			cy = rct->y = e.y;
			grid.nodes->addChild(rct);
		} else if (e.mk == WM_LDRAG_MK_MOVE) {
			grid.reset(e);
			if (e.x > cx) rct->width = e.x - cx + 1, rct->x = cx;
			else rct->width = cx - e.x + 1, rct->x = e.x;
			if (e.y > cy) rct->height = e.y - cy + 1, rct->y = cy;
			else rct->height = cy - e.y + 1, rct->y = e.y;
		} else if (e.mk == WM_LDRAG_MK_END) {
			rct->color = get_next_color();
			rct = nullptr;
			level = 0;
		}
	}

	bool activate() override {
		stage.mouseEventListener.add<CreateNode>(this, WM_LDRAG, &CreateNode::respond);
		return true;
	}

	bool deactivate() override {
		if (level) return false;
		stage.mouseEventListener.remove(this);
		return true;
	}
};

ToolBox<SelectNode, CreateNode> tb;


void Cmd(KeyboardEvent e) {
	if (stage.console_show || e.keyCode != (int)'C') return;
	stage.showConsole();
	SCommand<> sc;
	bool hold = false;
	do {
		if (!sc.read() || sc.parse(L"exit")) break;
		else if (sc.parse(L"hold")) hold = true;
		else if (sc.parse(L"grid", L"-h")) grid.hide();
		else if (sc.parse(L"grid", L"-s")) grid.show();
		else if ((std::wcout << L"Invalid.\n", sc.read())) continue;
	} while (hold);
	stage.hideConsole();
}

void onKey(KeyboardEvent e) {
	switch (e.keyCode)
	{
	case 'D': tb.switch_to<CreateNode>(); break;
	case 'S': tb.switch_to<SelectNode>(); break;
	case 'C': return Cmd(e);
	case VK_ESCAPE: PostMessage(System::g_hWnd, WM_DESTROY, 0, 0); break;
	default:
		break;
	}
}


void System::Setup() {
	stage.addConsole();
	SCommand<> sc;
	unsigned m = 0, n = 0, a = 0;
	std::wcout << L"input m,n,size of the grids:\n";
	while(!sc.read() || !sc.parse(m, n, a)) std::wcout << L"Invalid.\n";
	if (m * a < 200 || n * a < 200) m = n = 50, a = 15;
	grid.init(m, n, a);
	tb.init();
	stage.hideConsole();
	InitWindow(L"Program", 20, 20, m * a, n * a, WindowStyle(1, 0, 1, 0, 1, 0));
	stage.keyboardEventListener.add(WM_KEYUP, onKey);
}


#if 0
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
	ml->loadMMD(L"C:\\Users\\Ò¶¿ª\\Desktop\\3dmax\\model", L"mmd.obj", 12, true);
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
#if 0
	stage.addConsole();

	int yy = 2019, mm = 8, dd = 21;
	SPattern<3> sp(L"YY", L"MM", L"DD");
	sp.read(yy, mm, dd);
	std::wcout << sp.to_string(L"YY/MM/DD") << std::endl;
	sp.format(L"YY-MM-DD", L"2019-8-21");
	sp.write(yy, mm, dd);
	std::wcout << yy << L" " << mm << L" " << dd << std::endl;
#endif
}


#endif