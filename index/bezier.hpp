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

//�ṹ�壺��ά������
struct Point2D {
	int x, y;
};

//�ṹ�壺��ά��������
struct FPoint2D {
	scalar x, y;
};

//�����ࣺ�������ͻ��Ʊ���������
class BezierPainterImpl {
private:
	//���ƶ����Ĭ��Ϊ��ɫ
	static constexpr DWORD poly_color = Color::GREEN;
	
	Bitmap bmp; //�����λͼ
	std::vector<Point2D> vertex; //���ƶ���εĶ�������
	int grid_w, grid_h; //����Ŀ�͸�

	//�������������һ��(gx,gy)������Ϊcolor��ɫ
	void fill(int gx, int gy, int color) {
		if (!(gx < 0 || gx >= grid_w || gy < 0 || gy >= grid_h)) {
			DWORD* wr = bmp->src + gx + gy * grid_w;
			wr[0] = color;
		}
	}

	//������������x0,y0����x1,y1�����һ����ɫΪpcolor��ֱ��
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

	//�������������һ������εıհ������߶��Ƿ�С����ֵ
	//�����εĵ㼯P0,P1,...,Pn�������߶ȵķ���Ϊ h = max{ ��Pi��ֱ��P0Pn�ľ��� }������1<=i<=n-1
	//�������Pi��ֱ��P0Pn�ľ��룬ͨ����P0Pi��P0Pn�Ĳ�������ƽ���ı��ε����������P0Pn�ĳ��ȣ����õ��㵽ֱ�ߵľ���
	//����һ��������ֵ��0.5���أ�������false�����򷵻�true
	bool check_maxh(const std::vector<FPoint2D>& vec) {
		int len = (int)vec.size();
		//ֱ�ߺ͵�ĸ߶�Ϊ0
		if (len < 3) return true;

		//P0Pn����
		FPoint2D vec0n = { vec[0].x - vec[len - 1].x,vec[0].y - vec[len - 1].y };
		//����Ĳο�ֵ����ͨ����ʽ d * |P0Pn| = P0Pi x P0Pn �Ƶ���cmp_val = d * |P0Pn|
		//ֻҪP0Pi�Ĳ������cmp_val��˵���߶ȳ�������ֵd��0.5���أ�
		scalar cmp_val = (scalar)(0.5 * illSqrt(vec0n.x * vec0n.x + vec0n.y * vec0n.y));
		for (int i = 1; i + 1 < len; ++i) {
			scalar dx = vec[i].x - vec[0].x;
			scalar dy = vec[i].y - vec[0].y;
			if (illFabs(dx * vec0n.y - dy * vec0n.x) > cmp_val) return false; //�Ƚϲ��
		}
		return true;
	}

	//�������������Ʊ��������ߵ���ɫ
	static DWORD& color() { static DWORD instance = 0xff0000; return instance; }


public:
	//����ֵ���ֱ��ʾ�Ƿ�����˻��ơ��Ƿ���Ҫ��ʾ�����
	bool fin, show_poly;

	//���캯�������л����ĳ�ʼ��
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
	//�����������ͷ���Դ
	~BezierPainterImpl() { bmp->src = nullptr; }


	//���������ߵĻ��ƺ���
	void paint() {
		//��������Ϊ�գ����û���
		if (vertex.empty()) return;
		//��ʼ������
		memset(bmp->src, ~0, bmp->width * bmp->height << 2);

		//����ά������ת��Ϊ��ά��������
		std::vector<FPoint2D> fvertex(vertex.size());
		for (int i = 0, len = (int)fvertex.size(); i < len; ++i) {
			fvertex[i].x = (scalar)vertex[i].x;
			fvertex[i].y = (scalar)vertex[i].y;
		}
		//����ջ
		std::stack<std::vector<FPoint2D>> stk;
		stk.push(fvertex);
		//��ջ�ǿգ����������㷨
		while (!stk.empty()) {
			std::vector<FPoint2D>& p = stk.top();
			if (check_maxh(p)) {
				//������ƶ���εıհ��߶�С����ֵ0.5���ͻ�������ֱ�ߣ�������������Ǳ�֤��ϸǡ��
				Bresenham((int)p.front().x, (int)p.front().y, (int)p.back().x, (int)p.back().y, color());
				Bresenham((int)p.front().x + 1, (int)p.front().y, (int)p.back().x + 1, (int)p.back().y, color());
				Bresenham((int)p.front().x, (int)p.front().y + 1, (int)p.back().x, (int)p.back().y + 1, color());
				Bresenham((int)p.front().x + 1, (int)p.front().y + 1, (int)p.back().x + 1, (int)p.back().y + 1, color());
				stk.pop();
			} else {
				//���򣬶��ڿ��ƶ���ν�һ���ָ�����ƶ����ϸ�ֳ��ȼ۵������µĿ��ƶ����
				//�����������㷨��ͬ
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

		//�������Ҫ��ʾ���ƶ���Σ���������
		if (!show_poly) return;
		//���򣬻��ƿ��ƶ����
		for (int i = -2; i <= 2; ++i) {
			for (int j = -2; j <= 2; ++j) fill(vertex.begin()->x + i, stage.height - 1 - vertex.begin()->y + j, poly_color);
		}
		for (std::vector<Point2D>::iterator p = vertex.begin(), s = p++; p != vertex.end(); s = p++) {
			//������ֱ�߻��ƣ���֤��ϸǡ��
			Bresenham(s->x, s->y, p->x, p->y, poly_color);
			Bresenham(s->x + 1, s->y + 1, p->x + 1, p->y + 1, poly_color);
			Bresenham(s->x + 1, s->y, p->x + 1, p->y, poly_color);
			Bresenham(s->x, s->y + 1, p->x, p->y + 1, poly_color);
			for (int i = -2; i <= 2; ++i) {
				for (int j = -2; j <= 2; ++j) fill(p->x + i, stage.height - 1 - p->y + j, poly_color);
			}
		}
	}

	//��Ӧ����ƶ����϶��¼�
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

	//��Ӧ������¼�
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

	//��Ӧ�����¼�
	void onKey(KeyboardEvent e) {
		if (e.keyCode == 'C') color() = RGB3D(rand(), rand(), rand());
		else if (!fin && e.keyCode == 'B' && !vertex.empty()) vertex.pop_back();
		else if (fin && e.keyCode == 'P') show_poly = !show_poly;
		else return;
		paint();
	}

};


//ʵʱ��ʾ���λ��
void onPaint(SimpleEvent<STextImpl&> stxt) {
	stxt.value.caption.str(L"");
	stxt.value.caption << L"mouse: " << stage.mouseX << L',' << stage.mouseY;
}

//����������
sptr<BezierPainterImpl> bp;

//��ʾ������Ϣ
void onInfo(SimpleEvent<STextImpl&> stxt) {
	if (!bp->show_poly) stxt.value.caption.str(L"����P�˳�����ģʽ��������ʾ���ƶ����\n����C���������ɫ");
	else if (bp->fin) stxt.value.caption.str(L"����϶��ɿ��ƶ���ζ��㣬����λ���Ҽ���ɾ��ͼ��\n����P���ؿ��ƶ���Σ�����C���������ɫ");
	else stxt.value.caption.str(L"�����Ӷ���ζ��㣬�Ҽ���Ӷ��㲢��ֹ����\n����C���������ɫ������B������һ������");
}



//���������������ʼ��
void System::Setup() {
	//��ʼ������
	InitWindow(L"������������ʾ����", 20, 20, 800, 600, WindowStyle(1, 0, 1, 0, 1, 0));
	bp = sptr<BezierPainterImpl>(new BezierPainterImpl);
	SText tool_info = MakeSText(40, 550, L"");
	//��ʼ����ʾ���λ����Ϣ
	SText pos_text = MakeSText(640, 560, L"mouse: ", SFont(20));
	//ע������¼����Լ�������Ԫ����ӵ���̨
	tool_info->paintEventListener.add(0, onInfo);
	pos_text->paintEventListener.add(0, onPaint);
	stage.addChild(pos_text);
	stage.addChild(tool_info);
	stage.showFrameDelay(false);
}