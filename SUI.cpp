#include "SUI.h"
#include "Stage.h"
#include "SObject3D.h"
#include "SText.h"

extern fl::display::Stage stage;
extern HWND g_hWnd;



fl::ui::Roamer::Roamer(geom::Stage3D* link, int move_speed, int rotate_speed) :
	link(link), speed(move_speed), rad(0.01 * rotate_speed), centerX(link->width >> 1), centerY(link->height >> 1),
	x(0), y(0)
{
	stage.keyboardEventListener.add(this, WM_KEYDOWN, &Roamer::keyDown);
	stage.mouseEventListener.add(this, WM_MOUSEMOVE, &Roamer::mouseMove);
	stage.systemEventListener.add(this, WM_PAINT, &Roamer::update);
	ShowCursor(FALSE);
}

void fl::ui::Roamer::keyDown(events::KeyboardEvent e) {
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
		PostMessage(g_hWnd, WM_DESTROY, 0, 0);
	}
	break;
	}
}

void fl::ui::Roamer::mouseMove(events::MouseEvent e) {
	x = e.x;
	y = e.y;
}

void fl::ui::Roamer::update(events::SystemEvent e) {
	if (x > centerX + 2) link->camera.rotateH(rad);
	else if (x < centerX - 2) link->camera.rotateH(-rad);
	if (y > centerY + 2) link->camera.rotateV(-rad);
	else if (y < centerY - 2) link->camera.rotateV(rad);
	stage.setMouse(centerX, centerY);
	x = centerX;
	y = centerY;
}