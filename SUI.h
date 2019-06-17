#pragma once
#include "SPointer.h"
#include "Stage3D.h"
#include "Sprite.h"


namespace fl {

	namespace ui {

		class Roamer :public AutoPtr {
		public:
			Roamer(geom::Stage3D* link, int move_speed = 7, int rotate_speed = 5);
			void keyDown(events::KeyboardEvent e);
			void mouseMove(events::MouseEvent e);
			void update(events::SystemEvent e);
		private:
			geom::Stage3D* link;
			geom::Rad rad;
			int centerX, centerY;
			int x, y;
			int speed;
		};
	}
}