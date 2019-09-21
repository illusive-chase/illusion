#pragma once

#define import_all
#include "../src/top_element/SImport.h"
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
		for (int y = 1; y < m; ++y) grid->addChild(MakeSLine(0, y * a, n * a, y * a, 0, 1, RGB(0xc0c0c0)));
		for (int x = 1; x < n; ++x) grid->addChild(MakeSLine(x * a, 0, x * a, m * a, 0, 1, RGB(0xc0c0c0)));
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
	NodeImpl() :SRectImpl(0, 0, 1, 1, true, RGB(0xe8ecf1), PS_NULL), st(nullptr) {}
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
	void cancel_select() { sl->ps = PS_NULL; sl = nullptr; }

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
			for (auto it = grid.nodes->children.rbegin(); it != grid.nodes->children.rend(); it++) {
				if ((*it)->hitTestPoint(stage.mouseX, stage.mouseY)) {
					select(*it);
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
		if (sl) cancel_select();
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
				else if (sc.parse(L"c", r)) sl->color = RGB(r);
				else break;
			}
			stage.hideConsole();
			//SetFocus(System::g_hWnd);
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
		if (sl) cancel_select();
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
		constexpr int len = 6;
		static DWORD colors[len] = {
			0xf1a9a0,
			0xd5b8ff,
			0x89c4f4,
			0xc8f7c5,
			0xffffcc,
			0xfde3a7
		};
		static int t = 0;
		return RGB(colors[(t++) % len]);
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
	//SetFocus(System::g_hWnd);
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
	while (!sc.read() || !sc.parse(m, n, a)) std::wcout << L"Invalid.\n";
	if (m * a < 200 || n * a < 200) m = n = 50, a = 15;
	grid.init(m, n, a);
	tb.init();
	InitWindow(L"Program", 20, 20, m * a, n * a, WindowStyle(1, 0, 1, 0, 1, 0));
	stage.keyboardEventListener.add(WM_KEYUP, onKey);
	stage.hideConsole();
}