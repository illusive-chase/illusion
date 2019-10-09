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

class Grid {
private:
	
	SBitmap sbmp;
	const int width, height, size;
	DWORD* keep_src;
	DWORD* origin;

public:
	const int m, n, each_size;
	DWORD* src;

	Grid(int x, int y, int m, int n, int each_size) : sbmp(nullptr),
		width(n * (each_size + 1) + 1), height(m * (each_size + 1) + 1), size(width* height), keep_src(nullptr),
		m(m), n(n), each_size(each_size),
		src(nullptr)
	{
		sbmp = MakeSBitmap(x, y, MakeBitmap(width, height, nullptr));
		src = sbmp->content();
		memset(src, 0xff, size << 2);
		for (int i = 0; i < width; i += each_size + 1) {
			for (int j = 0; j < height; ++j) src[i + width * j] = RGB(0xc0c0c0);
		}
		for (int i = 0; i < width; ++i) {
			for (int j = 0; j < height; j += each_size + 1) src[i + width * j] = RGB(0xc0c0c0);
		}
		keep_src = new DWORD[size];
		origin = new DWORD[size];
		memcpy_s(origin, size << 2, src, size << 2);
		stage.addChild(sbmp);
	}

	Grid(const Grid& rhs) = delete;
	~Grid() { if (keep_src) delete keep_src; if (origin) delete origin; }

	void fillPixel(int x, int y, DWORD color) {
		if (x < 0 || y < 0 || x >= m || y >= n) return;
		y = n - 1 - y;
		x = x * (each_size + 1) + 1;
		y = y * (each_size + 1) + 1;
		DWORD* temp = src + (x + y * width);
		for (int i = 0; i < each_size; ++i) {
			for (int j = 0; j < each_size; ++j) temp[i + j * width] = color;
		}
	}

	void getFloatPos(double& x, double& y) {
		int mx = stage.mouseX, my = stage.mouseY;
		sbmp->transGlobalPosToLocal(mx, my);
		mx -= sbmp->x;
		my -= sbmp->y;
		x = double(mx) / (each_size + 1);
		y = double(my) / (each_size + 1);
	}

	void getIntPos(int& x, int& y) {
		int mx = stage.mouseX, my = stage.mouseY;
		sbmp->transGlobalPosToLocal(mx, my);
		mx -= sbmp->x;
		my -= sbmp->y;
		x = mx / (each_size + 1);
		y = my / (each_size + 1);
	}

	ILL_INLINE void restore() { memcpy_s(src, size << 2, keep_src, size << 2); }

	ILL_INLINE void keep() { memcpy_s(keep_src, size << 2, src, size << 2); }

	ILL_INLINE void restart() { memcpy_s(src, size << 2, origin, size << 2); }
	
};

sptr<Grid> grid{ nullptr };

class Algorithm {
public:
	static void Bresenham(int x0, int y0, int x1, int y1);
	static void Circle(int x0, int y0, float radius);
};

class LineTool :public ToolBase {
private:
	int level;
	int cx, cy;

public:
	LineTool() :level(0), cx(0), cy(0) {}
	void respond(MouseEvent e) {
		if (e.mk == WM_LDRAG_MK_BEGIN) {
			level = 1;
			grid->keep();
			grid->getIntPos(cx, cy);
		} else if (e.mk == WM_LDRAG_MK_MOVE) {
			grid->restore();
			grid->getIntPos(e.x, e.y);
			Algorithm::Bresenham(cx, cy, e.x, e.y);
		} else if (e.mk == WM_LDRAG_MK_END) {
			level = 0;
		}
	}

	bool activate() override {
		stage.mouseEventListener.add<LineTool>(this, WM_LDRAG, &LineTool::respond);
		return true;
	}

	bool deactivate() override {
		if (level) return false;
		stage.mouseEventListener.remove(this);
		return true;
	}
};

class CircleTool :public ToolBase {
private:
	int level;
	int cx, cy;

	ILL_INLINE float distance(int dx, int dy) {
		return illSqrt(dx * dx + dy * dy);
	}

public:
	CircleTool() :level(0), cx(0), cy(0) {}
	void respond(MouseEvent e) {
		if (e.mk == WM_LDRAG_MK_BEGIN) {
			level = 1;
			grid->keep();
			grid->getIntPos(cx, cy);
		} else if (e.mk == WM_LDRAG_MK_MOVE) {
			grid->restore();
			grid->getIntPos(e.x, e.y);
			Algorithm::Circle(cx, cy, distance(e.x - cx, e.y - cy));
		} else if (e.mk == WM_LDRAG_MK_END) {
			level = 0;
		}
	}

	bool activate() override {
		stage.mouseEventListener.add<CircleTool>(this, WM_LDRAG, &CircleTool::respond);
		return true;
	}

	bool deactivate() override {
		if (level) return false;
		stage.mouseEventListener.remove(this);
		return true;
	}
};

class ClearTool :public ToolBase {
public:
	bool activate() override {
		grid->restart();
		return false;
	}
	
	bool deactivate() override { return false; }
};

ToolBox<LineTool, CircleTool, ClearTool> tb;
SText tool_text;

void onLine(MouseEvent e) {
	if (tb.switch_to<LineTool>())
		tool_text->caption.str(L"当前工具：直线");
}

void onCircle(MouseEvent e) {
	if (tb.switch_to<CircleTool>())
		tool_text->caption.str(L"当前工具：圆周");
}

void onClear(MouseEvent e) {
	tb.switch_to<ClearTool>();
}

void onPaint(SimpleEvent<STextImpl&> stxt) {
	int x, y;
	grid->getIntPos(x, y);
	stxt.value.caption.str(L"");
	stxt.value.caption << L"mouse: " << x << L',' << y;
}

void System::Setup() {
	tb.init();
	stage.showFrameDelay(false);
	grid = sptr<Grid>(new Grid(80, 10, 150, 150, 4));

	SText pos_text = MakeSText(850, 735, L"mouse: ", SFont(24));
	pos_text->paintEventListener.add(0, onPaint);
	tool_text = MakeSText(850, 690, L"当前工具：直线", SFont(24));

	SButton btn_line = MakeSButton(850, 400, 100, 50, RGB(0xc0c0d0), RGB(0xd0d0f0), L"直线", 28, onLine);
	SButton btn_circle = MakeSButton(850, 500, 100, 50, RGB(0xc0c0d0), RGB(0xd0d0f0), L"圆周", 28, onCircle);
	SButton btn_clear = MakeSButton(850, 600, 100, 50, RGB(0xc0c0d0), RGB(0xd0d0f0), L"清除", 28, onClear);

	stage.addChild(btn_circle);
	stage.addChild(btn_line);
	stage.addChild(btn_clear);
	stage.addChild(pos_text);
	stage.addChild(tool_text);
	InitWindow(L"Program", 20, 20, 1024, 768, WindowStyle(1, 0, 1, 0, 1, 0));
}

void Algorithm::Bresenham(int x0, int y0, int x1, int y1) {
	int dx = x1 - x0;
	int dy = y1 - y0;

	int step_x = 1, step_y = 1, swap_flag = 0;
	if (dx < 0) dx = -dx, step_x = -1;
	if (dy < 0) dy = -dy, step_y = -1;

	if (dy > dx) std::swap(dx, dy), swap_flag = ~0;
	int f = (dy * 2) - dx;
	for (int i = 0; i <= dx; ++i) {
		grid->fillPixel(x0, y0, Color::RED);
		if (f >= 0) {
			x0 += step_x & swap_flag;
			y0 += step_y & ~swap_flag;
			f -= dx * 2;
		}
		f += dy * 2;
		x0 += step_x & ~swap_flag;
		y0 += step_y & swap_flag;
	}
}

void Algorithm::Circle(int x0, int y0, float radius) {
	int x = 0;
	int y = (int)(radius);
	int d = 12 - int(radius * 8);
	while (x < y) {
		grid->fillPixel(x0 + x, y0 + y, Color::RED);
		grid->fillPixel(x0 + x, y0 - y, Color::RED);
		grid->fillPixel(x0 - x, y0 + y, Color::RED);
		grid->fillPixel(x0 - x, y0 - y, Color::RED);
		grid->fillPixel(x0 + y, y0 + x, Color::RED);
		grid->fillPixel(x0 + y, y0 - x, Color::RED);
		grid->fillPixel(x0 - y, y0 + x, Color::RED);
		grid->fillPixel(x0 - y, y0 - x, Color::RED);
		if (d < 0) d += (x << 4) + 24;
		else {
			d += ((x - y) << 4) + 40;
			y--;
		}
		x++;
	}
	if (x == y) {
		grid->fillPixel(x0 + x, y0 + x, Color::RED);
		grid->fillPixel(x0 + x, y0 - x, Color::RED);
		grid->fillPixel(x0 - x, y0 - x, Color::RED);
		grid->fillPixel(x0 - x, y0 + x, Color::RED);
	}
}