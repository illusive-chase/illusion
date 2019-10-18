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

		// Class SpriteImpl is a container class.
		class SpriteImpl :public ShapeImpl {
		public:
			// Use vector to make it possible to get an object by index.
			// ATTENTION: There is an implicit convention that objects with smaller index 
			// will be overlaid on objects with larger indexes when drawing.
			std::vector<Shape> children;

			SpriteImpl(ShapeImpl* parent = nullptr) : ShapeImpl(parent) { x = 0, y = 0; width = height = 0; }
			SpriteImpl(int x, int y, int width, int height, ShapeImpl* parent = nullptr) : ShapeImpl(parent) {
				this->x = x, this->y = y;
				this->width = width, this->height = height;
			}
			virtual ~SpriteImpl() {}
			unsigned childrenNum() { return (unsigned)children.size(); }

			// insert
			void addChildAt(Shape shape, int index); 

			// That is, the newly added objects will be stacked on top.
			void addChild(Shape shape) { addChildAt(shape, 0); } 

			bool removeChildAt(int index);

			bool removeChild(Shape shape);

			void swapChild(int index1, int index2);

			// remove all children
			void clear();

			void disable() { for (Shape child : children) child->enabled = false; }
			void enable() { for (Shape child : children) child->enabled = true; }

			virtual void framing() override { for (Shape child : children) child->framing(); }
			virtual bool hitTestPoint(int x, int y) override;
			virtual void paint(HDC hdc) override;
		};

		using Sprite = sptr<SpriteImpl>;
		ILL_INLINE Sprite MakeSprite(ShapeImpl* parent = nullptr) { return Sprite(new SpriteImpl(parent)); }
		ILL_INLINE Sprite MakeSprite(int x, int y, int width, int height, ShapeImpl* parent = nullptr) { 
			return Sprite(new SpriteImpl(x, y, width, height, parent));
		}
	}
}

bool fl::display::SpriteImpl::removeChildAt(int index) {
	if (!children.size()) return false;
	index++;
	if (index < 1) index = 1;
	else if (index > (int)children.size()) index = (int)children.size();
	children.erase(children.end() - index);
	return true;
}

bool fl::display::SpriteImpl::removeChild(Shape shape)
{
	for (auto it = children.begin(); it != children.end(); it++) {
		if (*it == shape) return children.erase(it), true;
	}
	return false;
}

void fl::display::SpriteImpl::swapChild(int index1, int index2) {
	std::swap(children[index1], children[index2]);
}


void fl::display::SpriteImpl::addChildAt(Shape shape, int index) {
	shape->parent = this;
	if (index < 0) index = 0;
	else if (index > (int)children.size()) index = (int)children.size();
	children.insert(children.end() - index, shape);
}

void fl::display::SpriteImpl::clear() {
	children.clear();
}

bool fl::display::SpriteImpl::hitTestPoint(int x, int y) {
	for (Shape child : children) {
		if (child->hitTestPoint(x, y)) return true;
	}
	return false;
}

void fl::display::SpriteImpl::paint(HDC hdc) {
	if (visible) {
		if (width && height) {
			HDC memhdc = CreateCompatibleDC(hdc);
			for (Shape shape : children) shape->paint(memhdc);
			BitBlt(hdc, x, y, width, height, memhdc, x, y, SRCCOPY);
			DeleteDC(memhdc);
		} else for (Shape shape : children) shape->paint(hdc);
	}
}
