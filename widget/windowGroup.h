

#pragma once

#include "event.h"
#include "../view/view.h"
#include "../toolbox/container/list.h"



class WindowGroup : public VIEW::DRAWER {
public:
	//VIEW関連
	virtual void Draw();
	virtual void DrawTransparent();

	//イベント中継
	static void AtMouse(MOUSE_EVENT&);
	static void AtKey(KEY_EVENT&);
	static void AtJS(JS_EVENT&);
	static void AtSight(SIGHT_EVENT&);




private:
	wO::List<class Window> windows;
};

