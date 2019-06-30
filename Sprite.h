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

#include "Shape.h"

namespace fl {
	namespace display {

		// Class Sprite inherits class AutoPtr indirectly, which means it must be allocated on the heap.
		// Class Sprite implement the function of the container and can be inherited.
		class Sprite :public Shape {
		public:
			// Use vector to make it possible to get an object by index.
			// ATTENTION: There is an implicit convention that objects with smaller index 
			// will be overlaid on objects with larger indexes when drawing.
			std::vector<Shape*> children;

			Sprite(int x, int y, Shape* parent = nullptr) : Shape(parent) { this->x = x, this->y = y; width = height = 0; }
			virtual ~Sprite() { for (Shape* child : children) delete child; }
			unsigned childrenNum() { return (unsigned)children.size(); }

			// insert
			void addChildAt(Shape* shape, int index); 

			// That is, the newly added objects will be stacked on top.
			void addChild(Shape* shape) { addChildAt(shape, 0); } 

			// Actually, it also delete the child.
			bool removeChildAt(int index);

			void swapChild(int index1, int index2);

			// remove all children
			void clear();

			void disable() { for (Shape* child : children) child->enabled = false; }
			void enable() { for (Shape* child : children) child->enabled = true; }

			// It implements the corresponding virtual function of class Shape.
			virtual void framing() { for (Shape* child : children) child->framing(); }
			// It implements the corresponding virtual function of class Shape.
			virtual bool hitTestPoint(int x, int y);
			// It implements the corresponding virtual function of class Shape.
			virtual void paint(HDC hdc);
		};
	}
}