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