#pragma once

#include "Shape.h"

namespace fl {
	namespace display {
		class Sprite :public Shape {
		public:
			vector<Shape*> children;
			Sprite(int _x, int _y, Shape* parent = nullptr) : Shape(parent) { x = _x, y = _y; width = height = 0; }
			~Sprite() { for (Shape* child : children) delete child; }
			int childrenNum() { return (int)children.size(); }
			void addChildAt(Shape* shape, int index);
			void addChild(Shape* shape) { addChildAt(shape, 0); }
			bool removeChildAt(int index);
			void swapChild(int index1, int index2);
			void clear();
			void disable() { for (Shape* child : children) child->enabled = false; }
			void enable() { for (Shape* child : children) child->enabled = true; }
			virtual void framing() { for (Shape* child : children) child->framing(); }
			virtual bool hitTestPoint(int x, int y);
			virtual void paint(HDC hdc);
		};
	}
}