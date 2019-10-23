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

//帮助图标32x32的二进制表示
DWORD help_icon[32] = { 4293922815,4290774015,4278190335,4227858495,4161794079,4030725135,3774873095,3791650695,3287818115,3286239171,2279605217,2280653793,268108784,268371952,268177392,268181488,268189680,268189680,268435440,268320752,2281447393,2281185249,3288080323,3254665091,3791650695,3766484487,4030725135,4161794079,4227858495,4278190335,4290774015,4293922815 };

//结构：二维点
struct Point2D {
	int x, y;
};

//多边形类，负责保存多边形的顶点以及填充颜色、轮廓颜色等信息
class Poly {
private:
	std::list<Point2D> vertex; //顶点
public:
	DWORD color, pcolor; //颜色

	static DWORD& brush_color() { static DWORD instance = 0xff0000; return instance; }

	Poly() : vertex(), color(brush_color()), pcolor(0x0) {}

	void push(int x, int y) { vertex.push_back(Point2D{ x,y }); }

	void pop() { vertex.pop_back(); }

	std::list<Point2D>::iterator begin() { return vertex.begin(); }
	std::list<Point2D>::iterator end() { return vertex.end(); }
	bool empty() const { return vertex.empty(); }
	int size() const { return (int)vertex.size(); }

	void close() {
		vertex.push_back(*vertex.begin());
	}

	void open() {
		vertex.pop_back();
	}
	

	//测试x,y位置是否在多边形内，采用射线法判断交点个数奇偶性
	bool hitTestPoint(int x, int y) {
		if (vertex.size() < 3) return false;
		auto p = vertex.begin(), last = vertex.begin();
		bool hit = false;
		for (++p;; last = p++) {
			if (p == vertex.end()) p = vertex.begin();
			if (p->x < x && last->x > x && y * (p->x - last->x) - last->y * p->x + last->x * p->y - x * (p->y - last->y) > 0) hit = !hit;
			else if (p->x > x && last->x < x && y * (p->x - last->x) - last->y * p->x + last->x * p->y - x * (p->y - last->y) < 0) hit = !hit;
			if (p == vertex.begin()) break;
		}
		return hit;
	}

	//平移多边形
	void move(int dx, int dy) { for (Point2D& p : vertex) p.x += dx, p.y += dy; }
};


//多边形绘制类，负责实现多边形的填充算法，同时使用Bresenham算法绘制轮廓线
//它保存着窗口中所有多边形的列表
class PolyPainterImpl {
private:

	//结构：边
	struct Edge {
		int ym; //边的下端点y坐标（坐标系向下是y增大的方向）
		float x; //ET里表示边的上端点的横坐标，AEL里表示边与扫描线交点的横坐标
		float dx; //斜率的倒数
		Edge* next; //下一个Edge结构

		Edge() :ym(0), x(0), dx(0.0f), next(0) {}
		~Edge() { if (next) delete next; }
		bool operator <(const Edge& rhs) const {
			return (x == rhs.x) ? (dx < rhs.dx) : (x < rhs.x);
		}
	};

	Bitmap bmp, info;
	int info_x, info_y;

	std::list<Poly> polys; //多边形列表
	std::list<Poly>::iterator selected; //保存当前选取的多边形（轮廓必须绘制成青色）

	//向head列表中插入多边形add，并且会排序，插入到保序的位置
	void sorted_insert(Edge& head, Edge* add) {
		Edge* p = &head;
		for (; p->next && *p->next < *add; p = p->next);
		add->next = p->next;
		p->next = add;
	}

	//绘制水平的一系列像素，纵坐标是y，横坐标从begin到end，颜色是color，其中两端点颜色是pcolor
	void paint_line(int y, int begin, int end, DWORD color, DWORD pcolor) {
		if (y >= bmp->height || y < 0) return;
		y = bmp->height - y - 1;
		DWORD* write = bmp->src + y * bmp->width;
		if (0 <= begin && begin < bmp->width) write[begin] = pcolor;
		if (0 <= end && end < bmp->width) write[end] = pcolor;
		for (int i = max(0, begin + 1), len = min(end, bmp->width); i < len; ++i) write[i] = color;
	}

	//从x0,y0点向x1,y1点绘制一条颜色为pcolor的直线
	void Bresenham(int x0, int y0, int x1, int y1, DWORD pcolor) {
		y0 = bmp->height - 1 - y0;
		y1 = bmp->height - 1 - y1;
		int dx = x1 - x0;
		int dy = y1 - y0;

		int step_x = 1, step_y = 1, swap_flag = 0;
		if (dx < 0) dx = -dx, step_x = -1;
		if (dy < 0) dy = -dy, step_y = -1;

		if (dy > dx) std::swap(dx, dy), swap_flag = ~0;
		int f = dy * 2 - dx;
		for (int i = 0; i <= dx; ++i) {
			if (!(x0 < 0 || x0 >= bmp->width || y0 < 0 || y0 >= bmp->height)) bmp->src[x0 + y0 * bmp->width] = pcolor;
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

public:
	PolyPainterImpl() : polys(), selected(polys.end())
	{
		SBitmap sbmp = MakeSBitmap(0, 0, MakeBitmap(stage.width, stage.height, new DWORD[stage.width * stage.height]));
		bmp = MakeBitmap(sbmp->width, sbmp->height, sbmp->content());
		memset(bmp->src, ~0, bmp->width * bmp->height << 2);
		stage.addChild(sbmp);
	}

	~PolyPainterImpl() { bmp->src = nullptr; }

	void keep(Bitmap bmp, int x, int y) {
		info = bmp;
		info_x = x;
		info_y = y;
	}

	//主要算法函数，负责多边形填充
	void paint() {
		memset(bmp->src, ~0, bmp->width * bmp->height << 2);
		if (selected != polys.end()) selected->pcolor = Color::CYAN;

		//初始化ET和AEL
		Edge* edge_table = new Edge[stage.height];
		Edge* active_edge_list = new Edge;
		active_edge_list->x = INT_MIN;

		//遍历多边形列表里的每一个多边形
		for (auto& it = polys.rbegin(); it != polys.rend(); ++it) {
			Poly& p = *it; //用p变量保存当前遍历到的多边形
			if (p.size() < 3) continue; //顶点数小于3就不用绘制
			bool sl = false;
			if (selected != polys.end()) sl = true;
			p.close(); //向多边形中添加一个起始的顶点，保证多边形闭合
			//y_begin保存顶点中最小的y值，y_end保存顶点中最大的y值
			int y_begin = stage.height - 1, y_end = 0;
			
			auto& curr = p.begin(), last = p.begin();
			memset(edge_table, 0, sizeof(Edge) * stage.height); //设置ET为空

			//遍历多边形的顶点，curr保存当前遍历的顶点，last保存上一个遍历顶点
			for (curr++; curr != p.end(); last = curr++) {
				//如果last->curr的边是水平的，或者是超出了窗口大小就忽略这条边
				if (last->y == curr->y 
					|| (last->y >= stage.height && curr->y >= stage.height)
					|| (last->y < 0 && curr->y < 0)) continue;

				//add为将要加入到ET中的Edge
				Edge* add = new Edge;
				add->dx = float(last->x - curr->x) / (last->y - curr->y); //设置斜率倒数
				int y0, y1;
				//设置add起始的x,y和ym，且根据窗口大小裁剪add
				if (last->y < curr->y) {
					y1 = add->ym = min(curr->y, stage.height - 1);
					y0 = max(last->y, 0);
					add->x = float(last->x) + (y0 - last->y) * add->dx;
				} else if (last->y > curr->y) {
					y1 = add->ym = min(last->y, stage.height - 1);
					y0 = max(curr->y, 0);
					add->x = float(curr->x + (y0 - curr->y) * add->dx);
				}
				//有序地插入add到ET中去，索引为上端点的y坐标（坐标系向下是y增大的方向）
				sorted_insert(edge_table[y0], add);

				//设置顶点最大最小的y值
				y_begin = min(y_begin, y0);
				y_end = max(y_end, y1);
			}
			p.open(); //删除多边形的最后一个额外加入的顶点，恢复原状

			//根据窗口大小对y_begin,y_end裁剪
			y_begin = max(0, y_begin);
			y_end = min(y_end, stage.height - 1);

			//遍历扫描线的y值，从y_begin到y_end（这里递增的是y_begin，y_end作为结束值是不变的）
			for (; y_begin <= y_end; y_begin++) {
				//获取ET在y_begin索引下的Edge，存到et_p里
				Edge* et_p = edge_table[y_begin].next;
				edge_table[y_begin].next = nullptr; //删除原有的Edge
				Edge* ael_p = active_edge_list; //获取AEL起始的Edge，存到ael_p里
				Edge* last = nullptr; //last保存AEL中待配对的Edge

				//遍历AEL，这里调整了算法，使得我们一边向AEL插入ET中的Edge，一边遍历AEL
				for (; et_p || ael_p->next; ael_p = ael_p->next) {
					//选择ET中的Edge插入，或者让ael_p迭代到下一个Edge
					//这种选择取决于怎样能够让ET中的Edge插入到ael_p之后仍保序
					//由于我们同时插入和遍历，我们总是能通过条件判断来完成有序的插入
					if (et_p && (!ael_p->next || *et_p < *ael_p->next)) {
						Edge* temp = et_p;
						et_p = et_p->next;
						temp->next = ael_p->next;
						ael_p->next = temp;
					}
					//如果last为空，说明没有配对，那么令last为当前遍历的Edge
					if (!last) last = ael_p->next;
					else {
						//如果last非空，说明已经配对，那么根据配对的Edge绘制一条水平的线
						paint_line(y_begin, (int)round(last->x), (int)round(ael_p->next->x), p.color, p.pcolor);
						//绘制完成后，Edge的x递增dx
						last->x += last->dx;
						ael_p->next->x += ael_p->next->dx;
						//配对完成，last再次清空
						last = nullptr;
					}
				}
				//再次遍历AEL，这一次我们需要删除y+1=ym的边，同时更新x之后需要重新排序
				//对于非正常拓扑结构的多边形，重新遍历一次是必须的，
				//因为这种多边形的两条边相交的位置可能不存在顶点
				//而对于正常拓扑结构的多边形，可以合并到上一个循环，统一进行处理
				//这里当然需要考虑非正常拓扑，所以再遍历一次
				for (ael_p = active_edge_list; ael_p->next; ) {
					if (y_begin + 1 == ael_p->next->ym) {
						//ym = y + 1时，删除这个Edge
						Edge* temp = ael_p->next;
						ael_p->next = ael_p->next->next;
						temp->next = nullptr;
						delete temp;
						if (!ael_p->next) break;
					} else if (*ael_p->next < *ael_p || (ael_p->next->next && *ael_p->next->next < *ael_p->next)) {
						//如果发现Edge的x不再有序地递增，就取出逆序的Edge重新排序
						Edge* temp = ael_p->next;
						ael_p->next = ael_p->next->next;
						temp->next = nullptr;
						sorted_insert(*active_edge_list, temp);
					} else ael_p = ael_p->next;
				}
			}

			//再次遍历顶点，使用Bresenham算法绘制轮廓线
			curr = p.begin(), last = p.begin();
			for (curr++; curr != p.end(); last = curr++) Bresenham(curr->x, curr->y, last->x, last->y, it->pcolor);
			Bresenham(p.begin()->x, p.begin()->y, last->x, last->y, it->pcolor);
		}
		//删除ET和AEL
		delete[] edge_table;
		delete active_edge_list;

		//这里进行其他工作，包括实时调整多边形的颜色，以及帮助图标的绘制
		if (selected != polys.end()) selected->pcolor = Color::BLACK;
		for (int i = 0; i < info->width; ++i) {
			for (int j = 0; j < info->height; ++j) {
				DWORD info_color = info->src[i + j * info->width];
				if (info_color != 0xffffff)
					bmp->src[(bmp->height - j - info_y - 1) * bmp->width + i + info_x] = info_color;
			}
		}
	}

	void add(const Poly& poly) {
		polys.push_front(poly);
	}

	Poly& last() { return polys.front(); }

	void remove(std::list<Poly>::iterator it) {
		polys.erase(it);
	}

	std::list<Poly>::iterator select(int x, int y) {
		for (auto p = polys.begin(); p != polys.end(); ++p)
			if (p->hitTestPoint(x, y)) return selected = p;
		return selected = polys.end();
	}

	std::list<Poly>::iterator deselect() { return selected = polys.end(); }
};

using PolyPainter = sptr<PolyPainterImpl>;
PolyPainter pp; //声明一个多边形绘制类的实例
SText tool_info; //左下角的文字说明

//动画效果，文字说明的渐隐
void fade(FrameEvent e) {
	if (tool_info->alpha > 0xa0) tool_info->alpha--;
	else if (tool_info->alpha > 4) tool_info->alpha -= 4;
	else tool_info->alpha = 0;
}

//工具类：绘制多边形工具
class Creator :public ToolBase {
public:
	SEllipse level;
	Creator() :level(nullptr) {}

	bool activate() override {
		pp->add(Poly());
		stage.mouseEventListener.add(this, WM_LBUTTONDOWN, &Creator::add_vertex);
		stage.mouseEventListener.add(this, WM_MOUSEMOVE, &Creator::mouse_move);
		stage.mouseEventListener.add(this, WM_RBUTTONDOWN, &Creator::remove_vertex);
		stage.setCursor(IDC_CROSS);
		tool_info->caption.str(L"当前工具：绘制多边形(C)\n左键单击添加多边形顶点，单击起始点以完成一个多边形绘制；右键单击删除上一个顶点");
		tool_info->alpha = 0xfe;
		return true;
	}
	bool deactivate() override {
		if (level) return false;
		stage.mouseEventListener.remove(this, WM_LBUTTONDOWN);
		stage.mouseEventListener.remove(this, WM_MOUSEMOVE);
		stage.mouseEventListener.remove(this, WM_RBUTTONDOWN);
		stage.setCursor(IDC_ARROW);
		return true;
	}

	void mouse_move(MouseEvent e) {
		pp->last().push(e.x, e.y);
		pp->paint();
		pp->last().pop();
	}

	void add_vertex(MouseEvent e) {
		if (level && !pp->last().empty()) {
			int dx = e.x - pp->last().begin()->x;
			int dy = e.y - pp->last().begin()->y;
			if (dx * dx + dy * dy < 100) {
				pp->add(Poly());
				stage.removeChild(level);
				level = nullptr;
				return;
			}
		}
		pp->last().push(e.x, e.y);
		int lx = pp->last().begin()->x - 10, ly = pp->last().begin()->y - 10;
		if (!level) stage.addChild(level = MakeSEllipse(lx, ly, 20, 20, true, RGB(Color::LIME), PS_NULL)), level->alpha = 0xa0;
	}

	void remove_vertex(MouseEvent e) {
		if (!pp->last().empty()) {
			pp->last().pop();
			if (pp->last().empty()) stage.removeChild(level), level = nullptr;
		}
	}
};


//工具类：选取多边形工具
class Selector :public ToolBase {
	std::list<Poly>::iterator sl, empty;
	int rx, ry;

public:
	Selector() :sl(pp->deselect()), empty(pp->deselect()), rx(0), ry(0) {}

	void rclick(MouseEvent e) {
		if (sl != empty) {
			pp->remove(sl);
			sl = pp->deselect();
			pp->paint();
		}
	}

	void drag(MouseEvent e) {
		if (e.mk == WM_LDRAG_MK_BEGIN) sl = pp->select(rx = e.x, ry = e.y);
		else if (e.mk == WM_LDRAG_MK_MOVE && sl != empty) sl->move(e.x - rx, e.y - ry), rx = e.x, ry = e.y;
		else return stage.setCursor(IDC_ARROW), void();
		pp->paint();
		if (sl != empty) stage.setCursor(IDC_SIZEALL);
	}

	bool activate() override {
		stage.mouseEventListener.add(this, WM_LDRAG, &Selector::drag);
		stage.mouseEventListener.add(this, WM_RBUTTONDOWN, &Selector::rclick);
		tool_info->caption.str(L"当前工具：选择多边形(S)\n左键单击选择多边形，拖动以改变多边形位置；右键单击删除选中的多边形");
		tool_info->alpha = 0xfe;
		return true;
	}

	bool deactivate() override {
		sl = pp->deselect();
		pp->paint();
		stage.mouseEventListener.remove(this, WM_LDRAG);
		stage.mouseEventListener.remove(this, WM_RBUTTONDOWN);
		return true;
	}

	
};

//工具箱，负责各种工具的切换
ToolBox<Creator, Selector> tb;

//实时显示鼠标位置
void onPaint(SimpleEvent<STextImpl&> stxt) {
	stxt.value.caption.str(L"");
	stxt.value.caption << L"mouse: " << stage.mouseX << L',' << stage.mouseY;
}

//处理键盘输入
void onKey(KeyboardEvent e) {
	if (e.keyCode == 'C') tb.switch_to<Creator>();
	else if (e.keyCode == 'S') tb.switch_to<Selector>();
	else if (e.keyCode >= '1' && e.keyCode <= '5') {
		tool_info->caption.str(L"");
		tool_info->caption << L"当前画笔颜色：";
		switch (e.keyCode)
		{
		case '1':
			Poly::brush_color() = Color::RED;
			tool_info->caption << L"红色(1)";
			break;
		case '2':
			Poly::brush_color() = Color::GREEN;
			tool_info->caption << L"绿色(2)";
			break;
		case '3':
			Poly::brush_color() = Color::ORANGE;
			tool_info->caption << L"橙色(3)";
			break;
		case '4':
			Poly::brush_color() = Color::BLUE;
			tool_info->caption << L"蓝色(4)";
			break;
		case '5':
			Poly::brush_color() = Color::PINK;
			tool_info->caption << L"粉色(5)";
			break;
		}
		if (tb.is_active<Creator>()) pp->last().color = Poly::brush_color();
		tool_info->caption << L"\n数字快捷键1-5，对应画笔颜色红色、绿色、橙色、蓝色、粉色";
		tool_info->alpha = 0xfe;
	}
}

Bitmap info_bmp; //帮助图标

//检测鼠标是否滑过帮助图标
void onMove(MouseEvent e) {
	if (e.x >= 700 && e.x <= 700 + info_bmp->width && e.y >= 80 && e.y <= 80 + info_bmp->height) {
		tool_info->caption.str(L"使用帮助：\n快捷键C：绘制多边形；快捷键S：选择多边形；快捷键1-5：更改画笔颜色");
		tool_info->alpha = 0xfe;
	}
}

//启动函数，负责初始化
void System::Setup() {
	//初始化窗口
	InitWindow(L"多边形填充演示程序", 20, 20, 800, 600, WindowStyle(1, 0, 1, 0, 1, 0));

	//初始化显示鼠标位置信息
	SText pos_text = MakeSText(640, 560, L"mouse: ", SFont(20));
	//初始化多边形绘制类
	pp = PolyPainter(new PolyPainterImpl);
	//初始化帮助图标
	info_bmp = MakeBitmap(32, 32, new DWORD[1024]);
	for (int i = 0; i < 32; ++i) {
		DWORD val = help_icon[i];
		DWORD* write = &info_bmp->src[i << 5];
		for (int j = 31; ~j; --j) {
			write[j] = (val & 0x1) ? 0xffffff : 0;
			val >>= 1;
		}
	}
	pp->keep(info_bmp, 700, 80);
	//注册各种事件，以及将各种元件添加到舞台
	pos_text->paintEventListener.add(0, onPaint);
	stage.addChild(pos_text);
	stage.showFrameDelay(false);
	stage.keyboardEventListener.add(WM_KEYDOWN, onKey);
	stage.addChild(tool_info = MakeSText(40, 550, L""));
	stage.frameEventListener.add(WM_FRAME, fade);
	stage.mouseEventListener.add(WM_MOUSEMOVE, onMove);
	//工具箱初始化
	tb.init();
}
