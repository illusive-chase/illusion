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

#include "SObject3D.h"
#include "../display/SText.h"

namespace fl {
	namespace geom {

		// Class SText3D inherits class AutoPtr indirectly, which means it must be allocated on the heap.
		// Each SText3D object keeps a HFONT handle, which refers to a font.
		// Class SText3D inherits class SText, which may need to be improved because SText3D never uses SText::paintEventListener.
		class SText3D :public fl::display::SText {
		public:
			Vector3D pos; // the relative position to the coordinate space of the linked object
			SObject3D* obj; // the linked object 

			// It is used to listen for painting events.
			// In fact, the event is only responded when function Stage3D::render is called.
			// See Stage3D::render in Stage3D.cpp.
			fl::events::Signal<fl::events::SimpleEvent<SText3D*>> renderEventListener;

			SText3D(const Vector3D& pos, SObject3D* obj, const wstring& caption,
				const fl::display::SFont& sfont = fl::display::SFont(20), Shape* parent = nullptr)
				:SText(0, 0, caption, sfont, parent), pos(pos), obj(obj) {
			}

		};

	}
}