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
#include "../top_element/Stage.h"
#include "SObject3D.h"
#include "../display/SText.h"


namespace fl {

	namespace ui {

		class RoamerImpl {
		public:
			RoamerImpl(geom::Stage3D link, int move_speed = 7, int rotate_speed = 5);
			void keyDown(events::KeyboardEvent e);
			void mouseMove(events::MouseEvent e);
			void update(events::SystemEvent e);
		private:
			geom::Stage3D link;
			geom::Rad rad;
			int centerX, centerY;
			int x, y;
			int speed;
		};

		using Roamer = sptr<RoamerImpl>;
		ILL_INLINE Roamer MakeRoamer(geom::Stage3D link, int move_speed = 7, int rotate_speed = 5) {
			return Roamer(new RoamerImpl(link, move_speed, rotate_speed));
		}
	}
}


extern fl::display::Stage stage;



fl::ui::RoamerImpl::RoamerImpl(geom::Stage3D link, int move_speed, int rotate_speed) :
	link(link), speed(move_speed), rad(scalar(0.01) * rotate_speed), centerX(link->width >> 1), centerY(link->height >> 1),
	x(0), y(0)
{
	stage.keyboardEventListener.add(this, WM_KEYDOWN, &RoamerImpl::keyDown);
	stage.mouseEventListener.add(this, WM_MOUSEMOVE, &RoamerImpl::mouseMove);
	stage.systemEventListener.add(this, WM_PAINT, &RoamerImpl::update);
	ShowCursor(FALSE);
}

void fl::ui::RoamerImpl::keyDown(events::KeyboardEvent e) {
	fl::geom::Vector3D tmp;
	switch (e.keyCode) {
	case 'D':
	{
		link->camera.moveH(speed);
	}
	break;
	case 'A':
	{
		link->camera.moveH(-speed);
	}
	break;
	case 'W':
	{
		link->camera.moveD(speed);
	}
	break;
	case 'S':
	{
		link->camera.moveD(-speed);
	}
	break;
	case 'Q':
	{
		link->camera.moveV(speed);
	}
	break;
	case 'E':
	{
		link->camera.moveV(-speed);
	}
	break;
	case VK_ESCAPE:
	{
		PostMessage(fl::System::g_hWnd, WM_DESTROY, 0, 0);
	}
	break;
	}
}

void fl::ui::RoamerImpl::mouseMove(events::MouseEvent e) {
	x = e.x;
	y = e.y;
}

void fl::ui::RoamerImpl::update(events::SystemEvent e) {
	if (x > centerX + 2) link->camera.rotateH(rad);
	else if (x < centerX - 2) link->camera.rotateH(-rad);
	if (y > centerY + 2) link->camera.rotateV(-rad);
	else if (y < centerY - 2) link->camera.rotateV(rad);
	stage.setMouse(centerX, centerY);
	x = centerX;
	y = centerY;
}