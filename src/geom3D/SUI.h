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
#include "../top_element/SPointer.h"
#include "Stage3D.h"
#include "../display/Sprite.h"


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