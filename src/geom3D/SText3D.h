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
		
		class SText3DImpl;

		using SText3D= sptr<SText3DImpl>;

		// Each SText3DImpl object keeps a HFONT handle, which refers to a font.
		// Class SText3DImpl inherits class STextImpl, 
		// which may need to be improved because SText3DImpl never uses STextImpl::paintEventListener.
		class SText3DImpl :public fl::display::STextImpl {
		public:
			Vector3D pos; // the relative position to the coordinate space of the linked object
			SObject3D obj; // the linked object 

			// It is used to listen for painting events.
			// In fact, the event is only responded when function Stage3DImpl::render is called.
			// See Stage3DImpl::render in Stage3DImpl.cpp.
			fl::events::Signal<fl::events::SimpleEvent<SText3D>> renderEventListener;

			SText3DImpl(const Vector3D& pos, SObject3D obj, const wstring& caption,
				const fl::display::SFont& sfont = fl::display::SFont(20), fl::display::ShapeImpl* parent = nullptr)
				:STextImpl(0, 0, caption, sfont, parent), pos(pos), obj(obj) {
			}

		};

		ILL_INLINE SText3D MakeSText3D(const Vector3D& pos, SObject3D obj, const wstring& caption,
			const fl::display::SFont& sfont = fl::display::SFont(20), fl::display::ShapeImpl* parent = nullptr) {
			return SText3D(new SText3DImpl(pos, obj, caption, sfont, parent));
		}

	}
}