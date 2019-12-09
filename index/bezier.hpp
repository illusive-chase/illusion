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

//结构体：二维整数点
struct Point2D {
	int x, y;
};

//结构体：二维浮点数点
struct FPoint2D {
	scalar x, y;
};

//画板类：负责管理和绘制贝塞尔曲线
class BezierPainterImpl {
private:
	//控制多边形默认为绿色
	static constexpr DWORD poly_color = Color::GREEN;
	
	Bitmap bmp; //缓存的位图
	std::vector<Point2D> vertex; //控制多边形的顶点数组
	int grid_w, grid_h; //画板的宽和高

	//辅助函数，填充一格(gx,gy)的像素为color颜色
	void fill(int gx, int gy, int color) {
		if (!(gx < 0 || gx >= grid_w || gy < 0 || gy >= grid_h)) {
			DWORD* wr = bmp->src + gx + gy * grid_w;
			wr[0] = color;
		}
	}

	//辅助函数，从x0,y0点向x1,y1点绘制一条颜色为pcolor的直线
	void Bresenham(int x0, int y0, int x1, int y1, DWORD pcolor) {
		y0 = grid_h - 1 - y0;
		y1 = grid_h - 1 - y1;
		int dx = x1 - x0;
		int dy = y1 - y0;

		int step_x = 1, step_y = 1, swap_flag = 0;
		if (dx < 0) dx = -dx, step_x = -1;
		if (dy < 0) dy = -dy, step_y = -1;

		if (dy > dx) std::swap(dx, dy), swap_flag = ~0;
		int f = dy * 2 - dx;
		for (int i = 0; i <= dx; ++i) {
			fill(x0, y0, pcolor);
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

	//辅助函数，检查一个多边形的闭包的最大高度是否小于阈值
	//设多边形的点集P0,P1,...,Pn，则计算高度的方法为 h = max{ 点Pi到直线P0Pn的距离 }，其中1<=i<=n-1
	//而计算点Pi到直线P0Pn的距离，通过求P0Pi与P0Pn的叉积，获得平行四边形的面积，除以P0Pn的长度，即得到点到直线的距离
	//距离一旦大于阈值（0.5像素），返回false，否则返回true
	bool check_maxh(const std::vector<FPoint2D>& vec) {
		int len = (int)vec.size();
		//直线和点的高度为0
		if (len < 3) return true;

		//P0Pn向量
		FPoint2D vec0n = { vec[0].x - vec[len - 1].x,vec[0].y - vec[len - 1].y };
		//叉积的参考值，它通过公式 d * |P0Pn| = P0Pi x P0Pn 推导，cmp_val = d * |P0Pn|
		//只要P0Pi的叉积大于cmp_val，说明高度超过了阈值d（0.5像素）
		scalar cmp_val = (scalar)(0.5 * illSqrt(vec0n.x * vec0n.x + vec0n.y * vec0n.y));
		for (int i = 1; i + 1 < len; ++i) {
			scalar dx = vec[i].x - vec[0].x;
			scalar dy = vec[i].y - vec[0].y;
			if (illFabs(dx * vec0n.y - dy * vec0n.x) > cmp_val) return false; //比较叉积
		}
		return true;
	}

	//辅助函数，控制贝塞尔曲线的颜色
	static DWORD& color() { static DWORD instance = 0xff0000; return instance; }


public:
	//布尔值，分别表示是否完成了绘制、是否需要显示多边形
	bool fin, show_poly;

	//构造函数，进行基本的初始化
	BezierPainterImpl() :fin(false), show_poly(true), grid_w(stage.width), grid_h(stage.height) {
		SBitmap sbmp = MakeSBitmap(0, 0, MakeBitmap(stage.width, stage.height, new DWORD[stage.width * stage.height]));
		bmp = MakeBitmap(sbmp->width, sbmp->height, sbmp->content());
		memset(bmp->src, ~0, bmp->width * bmp->height << 2);
		stage.addChild(sbmp);
		stage.mouseEventListener.add(this, WM_MOUSEMOVE, &BezierPainterImpl::onMove);
		stage.mouseEventListener.add(this, WM_LDRAG, &BezierPainterImpl::onMove);
		stage.mouseEventListener.add(this, WM_LBUTTONDOWN, &BezierPainterImpl::onClick);
		stage.mouseEventListener.add(this, WM_RBUTTONDOWN, &BezierPainterImpl::onClick);
		stage.keyboardEventListener.add(this, WM_KEYDOWN, &BezierPainterImpl::onKey);
	}
	//析构函数，释放资源
	~BezierPainterImpl() { bmp->src = nullptr; }


	//贝塞尔曲线的绘制函数
	void paint() {
		//如果多边形为空，不用绘制
		if (vertex.empty()) return;
		//初始化画板
		memset(bmp->src, ~0, bmp->width * bmp->height << 2);

		//将二维整数点转化为二维浮点数点
		std::vector<FPoint2D> fvertex(vertex.size());
		for (int i = 0, len = (int)fvertex.size(); i < len; ++i) {
			fvertex[i].x = (scalar)vertex[i].x;
			fvertex[i].y = (scalar)vertex[i].y;
		}
		//建立栈
		std::stack<std::vector<FPoint2D>> stk;
		stk.push(fvertex);
		//当栈非空，持续运行算法
		while (!stk.empty()) {
			std::vector<FPoint2D>& p = stk.top();
			if (check_maxh(p)) {
				//如果控制多边形的闭包高度小于阈值0.5，就绘制四条直线，这里绘制四条是保证粗细恰当
				Bresenham((int)p.front().x, (int)p.front().y, (int)p.back().x, (int)p.back().y, color());
				Bresenham((int)p.front().x + 1, (int)p.front().y, (int)p.back().x + 1, (int)p.back().y, color());
				Bresenham((int)p.front().x, (int)p.front().y + 1, (int)p.back().x, (int)p.back().y + 1, color());
				Bresenham((int)p.front().x + 1, (int)p.front().y + 1, (int)p.back().x + 1, (int)p.back().y + 1, color());
				stk.pop();
			} else {
				//否则，对于控制多边形进一步分割，将控制多边形细分出等价的两个新的控制多边形
				//这里与书上算法相同
				std::vector<FPoint2D> r(p.size());
				int len = (int)p.size();
				for (int i = 1; i < len; ++i) {
					r[len - i] = p[len - 1];
					for (int j = len - 1; j >= i; --j) {
						p[j].x = (p[j].x + p[j - 1].x) * scalar(0.5);
						p[j].y = (p[j].y + p[j - 1].y) * scalar(0.5);
					}
				}
				r[0].x = p[len - 1].x;
				r[0].y = p[len - 1].y;
				stk.push(r);
			}
		}

		//如果不需要显示控制多边形，结束绘制
		if (!show_poly) return;
		//否则，绘制控制多边形
		for (int i = -2; i <= 2; ++i) {
			for (int j = -2; j <= 2; ++j) fill(vertex.begin()->x + i, stage.height - 1 - vertex.begin()->y + j, poly_color);
		}
		for (std::vector<Point2D>::iterator p = vertex.begin(), s = p++; p != vertex.end(); s = p++) {
			//用四条直线绘制，保证粗细恰当
			Bresenham(s->x, s->y, p->x, p->y, poly_color);
			Bresenham(s->x + 1, s->y + 1, p->x + 1, p->y + 1, poly_color);
			Bresenham(s->x + 1, s->y, p->x + 1, p->y, poly_color);
			Bresenham(s->x, s->y + 1, p->x, p->y + 1, poly_color);
			for (int i = -2; i <= 2; ++i) {
				for (int j = -2; j <= 2; ++j) fill(p->x + i, stage.height - 1 - p->y + j, poly_color);
			}
		}
	}

	//响应鼠标移动、拖动事件
	void onMove(MouseEvent e) {
		static int cx = 0, cy = 0;
		static Point2D* pt = nullptr;
		if (fin) {
			stage.setCursor(IDC_ARROW);
			if (!show_poly) {
				pt = nullptr;
				return;
			}
			if (!pt) {
				for (Point2D& p : vertex) {
					int dx = p.x - e.x;
					int dy = p.y - e.y;
					if (dx * dx + dy * dy < 80) {
						stage.setCursor(IDC_SIZEALL);
						if (e.type == WM_MOUSEMOVE) return;
						if (e.mk == WM_LDRAG_MK_BEGIN) {
							cx = e.x;
							cy = e.y;
							pt = &p;
						}
						return;
					}
				}
			} else {
				stage.setCursor(IDC_SIZEALL);
				if (e.type == WM_MOUSEMOVE) return;
				if (e.mk == WM_LDRAG_MK_END) pt = nullptr;
				else if (e.mk == WM_LDRAG_MK_MOVE) {
					pt->x += e.x - cx;
					pt->y += e.y - cy;
					cx = e.x;
					cy = e.y;
					paint();
				}
			}
		} else {
			pt = nullptr;
			vertex.push_back(Point2D{ e.x,e.y });
			stage.setCursor(IDC_CROSS);
			paint();
			vertex.pop_back();
		}
	}

	//响应鼠标点击事件
	void onClick(MouseEvent e) {
		if (!fin) {
			vertex.push_back(Point2D{ e.x,e.y });
			if (e.type == WM_RBUTTONDOWN) fin = true;
		} else if (e.type == WM_RBUTTONDOWN && show_poly) {
			vertex.clear();
			fin = false;
			paint();
		}
	}

	//响应键盘事件
	void onKey(KeyboardEvent e) {
		if (e.keyCode == 'C') color() = RGB3D(rand(), rand(), rand());
		else if (!fin && e.keyCode == 'B' && !vertex.empty()) vertex.pop_back();
		else if (fin && e.keyCode == 'P') show_poly = !show_poly;
		else return;
		paint();
	}

};


//实时显示鼠标位置
void onPaint(SimpleEvent<STextImpl&> stxt) {
	stxt.value.caption.str(L"");
	stxt.value.caption << L"mouse: " << stage.mouseX << L',' << stage.mouseY;
}

//声明画板类
sptr<BezierPainterImpl> bp;

//显示帮助信息
void onInfo(SimpleEvent<STextImpl&> stxt) {
	if (!bp->show_poly) stxt.value.caption.str(L"键入P退出隐藏模式，重新显示控制多边形\n键入C随机更换颜色");
	else if (bp->fin) stxt.value.caption.str(L"左键拖动可控制多边形顶点，任意位置右键可删除图形\n键入P隐藏控制多边形，键入C随机更换颜色");
	else stxt.value.caption.str(L"左键添加多边形顶点，右键添加顶点并终止绘制\n键入C随机更换颜色，键入B撤销上一个顶点");
}



//启动函数，负责初始化
void System::Setup() {
	//初始化窗口
	InitWindow(L"贝塞尔曲线演示程序", 20, 20, 800, 600, WindowStyle(1, 0, 1, 0, 1, 0));
	bp = sptr<BezierPainterImpl>(new BezierPainterImpl);
	SText tool_info = MakeSText(40, 550, L"");
	//初始化显示鼠标位置信息
	SText pos_text = MakeSText(640, 560, L"mouse: ", SFont(20));
	//注册各种事件，以及将各种元件添加到舞台
	tool_info->paintEventListener.add(0, onInfo);
	pos_text->paintEventListener.add(0, onPaint);
	stage.addChild(pos_text);
	stage.addChild(tool_info);
	stage.showFrameDelay(false);
}