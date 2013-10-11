/***************************************************************** X eventloop
 *
 */

#include <stdio.h>
#include <string.h>

#include "xDisplay.h"
#include "window.h"



static int glxAttrs[] = {
	GLX_USE_GL,
	GLX_LEVEL, 0,
	GLX_RGBA,
	GLX_DOUBLEBUFFER,
	GLX_RED_SIZE, 8,
	GLX_GREEN_SIZE, 8,
	GLX_BLUE_SIZE, 8,
	GLX_ALPHA_SIZE, 8,
	GLX_DEPTH_SIZE, 24,
	GLX_STENCIL_SIZE, 8,
	GLX_ACCUM_RED_SIZE, 0,
	GLX_ACCUM_GREEN_SIZE, 0,
	GLX_ACCUM_BLUE_SIZE, 0,
	GLX_ACCUM_ALPHA_SIZE, 0,
	None
};


int XDISPLAY::damageBase;
int XDISPLAY::damage_err;



XDISPLAY::XDISPLAY() :
	xDisplay(XOpenDisplay("")){

	//根窓設定
#ifdef TEST
	rootWindowID = XCreateSimpleWindow(
		xDisplay,
		RootWindow(xDisplay, 0),
			0, 0,
			1280, 800,
			0,
			BlackPixel(xDisplay, 0),
			BlackPixel(xDisplay, 0));
	XMapWindow( xDisplay, rootWindowID );
	printf("rootWindowID:%d.\n", rootWindowID);
#else
	rootWindowID = RootWindow(xDisplay, 0);
#endif

	//根窓の大きさ取得
	Window dw;
	int di;
	unsigned du;
	XGetGeometry(
		xDisplay,
		rootWindowID,
		&dw, &di, &di,
		&width, &height,
		&du, &du);

	//rootのサブウインドウをキャプチャ
	XCompositeRedirectSubwindows(
		xDisplay, rootWindowID, CompositeRedirectManual);

	//エラーハンドラを設定
	XSetErrorHandler(XErrorHandler);

	//XDamageハンドラのための設定
	XDamageQueryExtension(xDisplay, &damageBase, &damage_err);

	//イベント選択
	XSelectInput(xDisplay, rootWindowID,
		SubstructureNotifyMask |
		StructureNotifyMask |
		PropertyChangeMask |
		ExposureMask |
		ButtonPressMask |
		ButtonReleaseMask |
		ButtonMotionMask |
		KeyPressMask |
		KeyReleaseMask);
	XFlush( xDisplay );
	XSync(xDisplay, false);


	//OpenGLに諸条件を設定
	XVisualInfo *visual =
	glXChooseVisual(xDisplay, DefaultScreen(xDisplay), glxAttrs);
	glxContext = glXCreateContext(xDisplay, visual, NULL, True);
	XFree(visual);

	//この根窓の描画条件をカレントにする
	Activate();
}


XDISPLAY::~XDISPLAY(){
	glXMakeCurrent(xDisplay, 0, NULL);
	glXDestroyContext(xDisplay, glxContext);
	XCompositeUnredirectSubwindows(
		xDisplay, rootWindowID, CompositeRedirectManual);
#ifdef TEST
	XUnmapWindow(xDisplay, rootWindowID);
	XDestroyWindow(xDisplay, rootWindowID);
#endif
}

int XDISPLAY::XErrorHandler(Display* d, XErrorEvent* e){
	printf("(%u)\nserial:%lu\nreqCode:%u\nminCode:%u\n",
	       (*e).error_code,
	       (*e).serial,
	       (*e).request_code,
	       (*e).minor_code);
	return 0;
}







void XDISPLAY::EventHandler(){
	XEvent e;
	while(XPending(xDisplay)){
		XNextEvent(xDisplay, &e);
		switch(e.type){
			case CreateNotify:
				if(e.xcreatewindow.window != rootWindowID){
					WINDOW::AtCreate(
						e.xcreatewindow,
						width, height);
				}
				break;
			case MapNotify:
				WINDOW::AtMap(e.xmap);
				break;
			case DestroyNotify:
				WINDOW::AtDestroy(e.xdestroywindow);
				break;
			case UnmapNotify:
				WINDOW::AtUnmap(e.xunmap);
				break;
			case KeyPress:
				static int window(-1);
				if(window < 0){
					window = XCreateSimpleWindow(
						xDisplay,
						rootWindowID,
						0,
						0,
						600,
						800,
						0,
						WhitePixel(xDisplay, 0),
						0x3C4048);
					XMapWindow(xDisplay, window);
					//描画テスト
					GC gc(XCreateGC(xDisplay, window, 0, 0));
					XSetForeground(xDisplay, gc, 0x00ff0000);
					XFillRectangle(xDisplay, window, gc, 100, 10, 200, 400);
					XSetForeground(xDisplay, gc, 0x000000ff);
					XFillArc(xDisplay, window, gc, 300, 400, 400, 400, 0, 360 * 64);
					XSetForeground(xDisplay, gc, 0xD2DEF0);
					XSetFont(xDisplay, gc, XLoadFont(xDisplay, "-*-*-*-*-*-*-24-*-*-*-*-*-iso8859-*"));
					const char* const str("First light!");
					XDrawString(xDisplay, window, gc, 200, 600, str, strlen(str));
					XFreeGC(xDisplay, gc);
				}else{
					XUnmapWindow(xDisplay, window);
					XDestroyWindow(xDisplay, window);
					window = -1;
				}
				break;
			//TODO:MouseDown/Upは座標を作った上で裏画面へ回送
			default:
				if(e.type == damageBase + XDamageNotify){
					//XDamageのイベント
					WINDOW::AtDamage(*(XDamageNotifyEvent*)&e);
				}else{
					//その他のイベント(裏画面へ転送)
				}
				break;
		}
	}
}
