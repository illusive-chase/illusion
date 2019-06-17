#pragma once

#include "SGeomMath.h"
#include "SText.h"

namespace fl {
	namespace geom {

		class SText3D :public fl::display::SText {

		public:
			Vector3D pos;
			fl::events::Signal<fl::events::SimpleEvent<SText3D*>> renderEventListener;

			SText3D(const Vector3D& pos, const wstring& caption,
				const fl::display::SFont& sfont = fl::display::SFont(20), Shape* parent = nullptr)
				:SText(0, 0, caption, sfont, parent), pos(pos) {
			}

		};

	}
}