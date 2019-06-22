#pragma once

#include "Sprite.h"
#include "STimer.h"
#include "SMovieClip.h"
using namespace fl::events;
using namespace fl::time;

namespace fl {
	namespace display {

		class Stage :public Sprite {
		public:
			Signal<KeyboardEvent> keyboardEventListener;
			Signal<MouseEvent> mouseEventListener;
			Signal<SystemEvent> systemEventListener;
			Signal<FrameEvent> frameEventListener;
			std::list<AutoPtr*> recycleListener;


			bool console_show;
			int mouseX, mouseY;

			Stage() :Sprite(0, 0, nullptr), mouseX(0), mouseY(0), console_show(false) {}
			~Stage() {}
			bool addConsole(const wstring& TITLE = L"Console");
			bool showConsole();
			bool hideConsole();

			void destroy();
			void framing();

			RECT getWindowArea() const;
			RECT getStageArea() const;
			void setMouse(int x, int y);

			inline void addRecycleListener(AutoPtr * obj) {
				recycleListener.push_back(obj);
			}
			inline void removeRecycleListener(AutoPtr * obj) {
				for (auto it = recycleListener.begin(); it != recycleListener.end();) {
					if (*it == obj) it = recycleListener.erase(it);
					else ++it;
				}
			}

			//return ID**
			DWORD message(const wstring& test, const wstring& TITLE, UINT mode_MB, bool fixed);

			void removeEventListener();
			inline bool hitTestPoint(int x, int y) { return true; }
			void paint(HDC hdc);
		};


	}
}