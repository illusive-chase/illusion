#include "Sprite.h"



bool fl::display::Sprite::removeChildAt(int index) {
	if (!children.size()) return false;
	index++;
	if (index < 1) index = 1;
	else if (index > (int)children.size()) index = (int)children.size();
	delete children[(int)children.size() - index];
	children.erase(children.end() - index);
	return true;
}

void fl::display::Sprite::swapChild(int index1, int index2) {
	std::swap(children[index1], children[index2]);
}


void fl::display::Sprite::addChildAt(Shape* shape, int index) {
	shape->parent = this;
	if (index < 0) index = 0;
	else if (index > (int)children.size()) index = (int)children.size();
	children.insert(children.end() - index, shape);
}

void fl::display::Sprite::clear() {
	for (Shape* shape : children) delete shape;
	children.clear();
}

bool fl::display::Sprite::hitTestPoint(int x, int y) {
	for (Shape* child : children) {
		if (child->hitTestPoint(x, y)) return true;
	}
	return false;
}

void fl::display::Sprite::paint(HDC hdc) {
	if (visible) {
		for (Shape* shape : children) shape->paint(hdc);
	}
}
