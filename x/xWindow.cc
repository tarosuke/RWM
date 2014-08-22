#include "xWindow.h"

void XWINDOW::AtXCreate(const XCreateWindowEvent&, unsigned, unsigned){}
void XWINDOW::AtXCreate(Display* const, Window, unsigned, unsigned){}
void XWINDOW::AtXDestroy(const XDestroyWindowEvent&){}
void XWINDOW::AtXMap(const XMapEvent&){}
void XWINDOW::AtXUnmap(const XUnmapEvent&){}
void XWINDOW::AtXDamage(const XEvent&){}
