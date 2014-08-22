#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xdamage.h>

#include "../window/window.h"


class XWINDOW : public WINDOW{
	XWINDOW();
public:
	XWINDOW(float x, float y, int w, int h, Window, Display*);

	//X用イベントハンドラ
	static void AtXCreate(const XCreateWindowEvent&, unsigned, unsigned);
	static void AtXCreate(Display* const, Window, unsigned, unsigned);
	static void AtXDestroy(const XDestroyWindowEvent&);
	static void AtXMap(const XMapEvent&);
	static void AtXUnmap(const XUnmapEvent&);
	static void AtXDamage(const XEvent&);
	static void AtXConfigure(const XConfigureEvent&);
};

