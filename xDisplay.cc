/***************************************************************** X eventloop
 *
 */

#include <stdio.h>

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
	width = DisplayWidth(xDisplay, 0);
	height = DisplayHeight(xDisplay, 0);


	//rootのサブウインドウをキャプチャ
	XCompositeRedirectSubwindows(
		xDisplay, rootWindowID, CompositeRedirectManual);

	//エラーハンドラを設定
	XSetErrorHandler(XErrorHandler);

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

	//スレッド軌道
	pthread_create(&evThread, NULL, XDISPLAY::EventLoopEntry, (void*)this);
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







void XDISPLAY::EventLoop(){
	XEvent e;

	for(;;){
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

