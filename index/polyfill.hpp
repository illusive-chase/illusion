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

//����ͼ��32x32�Ķ����Ʊ�ʾ
DWORD help_icon[32] = { 4293922815,4290774015,4278190335,4227858495,4161794079,4030725135,3774873095,3791650695,3287818115,3286239171,2279605217,2280653793,268108784,268371952,268177392,268181488,268189680,268189680,268435440,268320752,2281447393,2281185249,3288080323,3254665091,3791650695,3766484487,4030725135,4161794079,4227858495,4278190335,4290774015,4293922815 };

//�ṹ����ά��
struct Point2D {
	int x, y;
};

//������࣬���𱣴����εĶ����Լ������ɫ��������ɫ����Ϣ
class Poly {
private:
	std::list<Point2D> vertex; //����
public:
	DWORD color, pcolor; //��ɫ

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
	

	//����x,yλ���Ƿ��ڶ�����ڣ��������߷��жϽ��������ż��
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

	//ƽ�ƶ����
	void move(int dx, int dy) { for (Point2D& p : vertex) p.x += dx, p.y += dy; }
};


//����λ����࣬����ʵ�ֶ���ε�����㷨��ͬʱʹ��Bresenham�㷨����������
//�������Ŵ��������ж���ε��б�
class PolyPainterImpl {
private:

	//�ṹ����
	struct Edge {
		int ym; //�ߵ��¶˵�y���꣨����ϵ������y����ķ���
		float x; //ET���ʾ�ߵ��϶˵�ĺ����꣬AEL���ʾ����ɨ���߽���ĺ�����
		float dx; //б�ʵĵ���
		Edge* next; //��һ��Edge�ṹ

		Edge() :ym(0), x(0), dx(0.0f), next(0) {}
		~Edge() { if (next) delete next; }
		bool operator <(const Edge& rhs) const {
			return (x == rhs.x) ? (dx < rhs.dx) : (x < rhs.x);
		}
	};

	Bitmap bmp, info;
	int info_x, info_y;

	std::list<Poly> polys; //������б�
	std::list<Poly>::iterator selected; //���浱ǰѡȡ�Ķ���Σ�����������Ƴ���ɫ��

	//��head�б��в�������add�����һ����򣬲��뵽�����λ��
	void sorted_insert(Edge& head, Edge* add) {
		Edge* p = &head;
		for (; p->next && *p->next < *add; p = p->next);
		add->next = p->next;
		p->next = add;
	}

	//����ˮƽ��һϵ�����أ���������y���������begin��end����ɫ��color���������˵���ɫ��pcolor
	void paint_line(int y, int begin, int end, DWORD color, DWORD pcolor) {
		if (y >= bmp->height || y < 0) return;
		y = bmp->height - y - 1;
		DWORD* write = bmp->src + y * bmp->width;
		if (0 <= begin && begin < bmp->width) write[begin] = pcolor;
		if (0 <= end && end < bmp->width) write[end] = pcolor;
		for (int i = max(0, begin + 1), len = min(end, bmp->width); i < len; ++i) write[i] = color;
	}

	//��x0,y0����x1,y1�����һ����ɫΪpcolor��ֱ��
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

	//��Ҫ�㷨�����������������
	void paint() {
		memset(bmp->src, ~0, bmp->width * bmp->height << 2);
		if (selected != polys.end()) selected->pcolor = Color::CYAN;

		//��ʼ��ET��AEL
		Edge* edge_table = new Edge[stage.height];
		Edge* active_edge_list = new Edge;
		active_edge_list->x = INT_MIN;

		//����������б����ÿһ�������
		for (auto& it = polys.rbegin(); it != polys.rend(); ++it) {
			Poly& p = *it; //��p�������浱ǰ�������Ķ����
			if (p.size() < 3) continue; //������С��3�Ͳ��û���
			bool sl = false;
			if (selected != polys.end()) sl = true;
			p.close(); //�����������һ����ʼ�Ķ��㣬��֤����αպ�
			//y_begin���涥������С��yֵ��y_end���涥��������yֵ
			int y_begin = stage.height - 1, y_end = 0;
			
			auto& curr = p.begin(), last = p.begin();
			memset(edge_table, 0, sizeof(Edge) * stage.height); //����ETΪ��

			//��������εĶ��㣬curr���浱ǰ�����Ķ��㣬last������һ����������
			for (curr++; curr != p.end(); last = curr++) {
				//���last->curr�ı���ˮƽ�ģ������ǳ����˴��ڴ�С�ͺ���������
				if (last->y == curr->y 
					|| (last->y >= stage.height && curr->y >= stage.height)
					|| (last->y < 0 && curr->y < 0)) continue;

				//addΪ��Ҫ���뵽ET�е�Edge
				Edge* add = new Edge;
				add->dx = float(last->x - curr->x) / (last->y - curr->y); //����б�ʵ���
				int y0, y1;
				//����add��ʼ��x,y��ym���Ҹ��ݴ��ڴ�С�ü�add
				if (last->y < curr->y) {
					y1 = add->ym = min(curr->y, stage.height - 1);
					y0 = max(last->y, 0);
					add->x = float(last->x) + (y0 - last->y) * add->dx;
				} else if (last->y > curr->y) {
					y1 = add->ym = min(last->y, stage.height - 1);
					y0 = max(curr->y, 0);
					add->x = float(curr->x + (y0 - curr->y) * add->dx);
				}
				//����ز���add��ET��ȥ������Ϊ�϶˵��y���꣨����ϵ������y����ķ���
				sorted_insert(edge_table[y0], add);

				//���ö��������С��yֵ
				y_begin = min(y_begin, y0);
				y_end = max(y_end, y1);
			}
			p.open(); //ɾ������ε����һ���������Ķ��㣬�ָ�ԭ״

			//���ݴ��ڴ�С��y_begin,y_end�ü�
			y_begin = max(0, y_begin);
			y_end = min(y_end, stage.height - 1);

			//����ɨ���ߵ�yֵ����y_begin��y_end�������������y_begin��y_end��Ϊ����ֵ�ǲ���ģ�
			for (; y_begin <= y_end; y_begin++) {
				//��ȡET��y_begin�����µ�Edge���浽et_p��
				Edge* et_p = edge_table[y_begin].next;
				edge_table[y_begin].next = nullptr; //ɾ��ԭ�е�Edge
				Edge* ael_p = active_edge_list; //��ȡAEL��ʼ��Edge���浽ael_p��
				Edge* last = nullptr; //last����AEL�д���Ե�Edge

				//����AEL������������㷨��ʹ������һ����AEL����ET�е�Edge��һ�߱���AEL
				for (; et_p || ael_p->next; ael_p = ael_p->next) {
					//ѡ��ET�е�Edge���룬������ael_p��������һ��Edge
					//����ѡ��ȡ���������ܹ���ET�е�Edge���뵽ael_p֮���Ա���
					//��������ͬʱ����ͱ���������������ͨ�������ж����������Ĳ���
					if (et_p && (!ael_p->next || *et_p < *ael_p->next)) {
						Edge* temp = et_p;
						et_p = et_p->next;
						temp->next = ael_p->next;
						ael_p->next = temp;
					}
					//���lastΪ�գ�˵��û����ԣ���ô��lastΪ��ǰ������Edge
					if (!last) last = ael_p->next;
					else {
						//���last�ǿգ�˵���Ѿ���ԣ���ô������Ե�Edge����һ��ˮƽ����
						paint_line(y_begin, (int)round(last->x), (int)round(ael_p->next->x), p.color, p.pcolor);
						//������ɺ�Edge��x����dx
						last->x += last->dx;
						ael_p->next->x += ael_p->next->dx;
						//�����ɣ�last�ٴ����
						last = nullptr;
					}
				}
				//�ٴα���AEL����һ��������Ҫɾ��y+1=ym�ıߣ�ͬʱ����x֮����Ҫ��������
				//���ڷ��������˽ṹ�Ķ���Σ����±���һ���Ǳ���ģ�
				//��Ϊ���ֶ���ε��������ཻ��λ�ÿ��ܲ����ڶ���
				//�������������˽ṹ�Ķ���Σ����Ժϲ�����һ��ѭ����ͳһ���д���
				//���ﵱȻ��Ҫ���Ƿ��������ˣ������ٱ���һ��
				for (ael_p = active_edge_list; ael_p->next; ) {
					if (y_begin + 1 == ael_p->next->ym) {
						//ym = y + 1ʱ��ɾ�����Edge
						Edge* temp = ael_p->next;
						ael_p->next = ael_p->next->next;
						temp->next = nullptr;
						delete temp;
						if (!ael_p->next) break;
					} else if (*ael_p->next < *ael_p || (ael_p->next->next && *ael_p->next->next < *ael_p->next)) {
						//�������Edge��x��������ص�������ȡ�������Edge��������
						Edge* temp = ael_p->next;
						ael_p->next = ael_p->next->next;
						temp->next = nullptr;
						sorted_insert(*active_edge_list, temp);
					} else ael_p = ael_p->next;
				}
			}

			//�ٴα������㣬ʹ��Bresenham�㷨����������
			curr = p.begin(), last = p.begin();
			for (curr++; curr != p.end(); last = curr++) Bresenham(curr->x, curr->y, last->x, last->y, it->pcolor);
			Bresenham(p.begin()->x, p.begin()->y, last->x, last->y, it->pcolor);
		}
		//ɾ��ET��AEL
		delete[] edge_table;
		delete active_edge_list;

		//���������������������ʵʱ��������ε���ɫ���Լ�����ͼ��Ļ���
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
PolyPainter pp; //����һ������λ������ʵ��
SText tool_info; //���½ǵ�����˵��

//����Ч��������˵���Ľ���
void fade(FrameEvent e) {
	if (tool_info->alpha > 0xa0) tool_info->alpha--;
	else if (tool_info->alpha > 4) tool_info->alpha -= 4;
	else tool_info->alpha = 0;
}

//�����ࣺ���ƶ���ι���
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
		tool_info->caption.str(L"��ǰ���ߣ����ƶ����(C)\n���������Ӷ���ζ��㣬������ʼ�������һ������λ��ƣ��Ҽ�����ɾ����һ������");
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


//�����ࣺѡȡ����ι���
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
		tool_info->caption.str(L"��ǰ���ߣ�ѡ������(S)\n�������ѡ�����Σ��϶��Ըı�����λ�ã��Ҽ�����ɾ��ѡ�еĶ����");
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

//�����䣬������ֹ��ߵ��л�
ToolBox<Creator, Selector> tb;

//ʵʱ��ʾ���λ��
void onPaint(SimpleEvent<STextImpl&> stxt) {
	stxt.value.caption.str(L"");
	stxt.value.caption << L"mouse: " << stage.mouseX << L',' << stage.mouseY;
}

//�����������
void onKey(KeyboardEvent e) {
	if (e.keyCode == 'C') tb.switch_to<Creator>();
	else if (e.keyCode == 'S') tb.switch_to<Selector>();
	else if (e.keyCode >= '1' && e.keyCode <= '5') {
		tool_info->caption.str(L"");
		tool_info->caption << L"��ǰ������ɫ��";
		switch (e.keyCode)
		{
		case '1':
			Poly::brush_color() = Color::RED;
			tool_info->caption << L"��ɫ(1)";
			break;
		case '2':
			Poly::brush_color() = Color::GREEN;
			tool_info->caption << L"��ɫ(2)";
			break;
		case '3':
			Poly::brush_color() = Color::ORANGE;
			tool_info->caption << L"��ɫ(3)";
			break;
		case '4':
			Poly::brush_color() = Color::BLUE;
			tool_info->caption << L"��ɫ(4)";
			break;
		case '5':
			Poly::brush_color() = Color::PINK;
			tool_info->caption << L"��ɫ(5)";
			break;
		}
		if (tb.is_active<Creator>()) pp->last().color = Poly::brush_color();
		tool_info->caption << L"\n���ֿ�ݼ�1-5����Ӧ������ɫ��ɫ����ɫ����ɫ����ɫ����ɫ";
		tool_info->alpha = 0xfe;
	}
}

Bitmap info_bmp; //����ͼ��

//�������Ƿ񻬹�����ͼ��
void onMove(MouseEvent e) {
	if (e.x >= 700 && e.x <= 700 + info_bmp->width && e.y >= 80 && e.y <= 80 + info_bmp->height) {
		tool_info->caption.str(L"ʹ�ð�����\n��ݼ�C�����ƶ���Σ���ݼ�S��ѡ�����Σ���ݼ�1-5�����Ļ�����ɫ");
		tool_info->alpha = 0xfe;
	}
}

//���������������ʼ��
void System::Setup() {
	//��ʼ������
	InitWindow(L"����������ʾ����", 20, 20, 800, 600, WindowStyle(1, 0, 1, 0, 1, 0));

	//��ʼ����ʾ���λ����Ϣ
	SText pos_text = MakeSText(640, 560, L"mouse: ", SFont(20));
	//��ʼ������λ�����
	pp = PolyPainter(new PolyPainterImpl);
	//��ʼ������ͼ��
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
	//ע������¼����Լ�������Ԫ����ӵ���̨
	pos_text->paintEventListener.add(0, onPaint);
	stage.addChild(pos_text);
	stage.showFrameDelay(false);
	stage.keyboardEventListener.add(WM_KEYDOWN, onKey);
	stage.addChild(tool_info = MakeSText(40, 550, L""));
	stage.frameEventListener.add(WM_FRAME, fade);
	stage.mouseEventListener.add(WM_MOUSEMOVE, onMove);
	//�������ʼ��
	tb.init();
}
