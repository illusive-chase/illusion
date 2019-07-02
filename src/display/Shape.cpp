#include "Shape.h"

void fl::display::Shape::transLocalPosToGlobal(int & x, int & y) {
	for (Shape* fa = parent; fa; fa = fa->parent) {
		x += fa->x;
		y += fa->y;
	}
}

void fl::display::Shape::transGlobalPosToLocal(int & x, int & y) {
	for (Shape* fa = parent; fa; fa = fa->parent) {
		x -= fa->x;
		y -= fa->y;
	}
}