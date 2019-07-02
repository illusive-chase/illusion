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

		// Class SpriteImpl implement the function of the container.
		class SpriteImpl :public ShapeImpl {
		public:
			// Use vector to make it possible to get an object by index.
			// ATTENTION: There is an implicit convention that objects with smaller index 
			// will be overlaid on objects with larger indexes when drawing.
			std::vector<Shape> children;

			SpriteImpl(int x, int y, Shape parent = nullptr) : ShapeImpl(parent) { this->x = x, this->y = y; width = height = 0; }
			virtual ~SpriteImpl() {}
			unsigned childrenNum() { return (unsigned)children.size(); }

			// insert
			void addChildAt(Shape shape, int index); 

			// That is, the newly added objects will be stacked on top.
			void addChild(Shape shape) { addChildAt(shape, 0); } 

			// Actually, it also delete the child.
			bool removeChildAt(int index);

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
		ILL_INLINE Sprite MakeSprite(int x, int y, Shape parent = nullptr) {
			return Sprite(new SpriteImpl(x, y, parent));
		}
	}
}