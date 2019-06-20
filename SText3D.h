#pragma once

#include "SObject3D.h"
#include "SText.h"

namespace fl {
	namespace geom {

		ILL_ATTRIBUTE_ALIGNED16(class) SText3D :public fl::display::SText {

		public:
			Vector3D pos;
			SObject3D* obj;
			fl::events::Signal<fl::events::SimpleEvent<SText3D*>> renderEventListener;

			SText3D(const Vector3D& pos, SObject3D* obj, const wstring& caption,
				const fl::display::SFont& sfont = fl::display::SFont(20), Shape* parent = nullptr)
				:SText(0, 0, caption, sfont, parent), pos(pos), obj(obj) {
			}

		};

	}
}