#include <pthread.h>

#include <X11/Xutil.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xcomposite.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <stdio.h>


#include "xDisplay.h"
#include "../view.h"



int XDISPLAY::glxAttributes[] = {
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


//根窓を取得してOpenGLとXDamageを含む各種イベントをセットアップ
XDISPLAY::XDISPLAY(unsigned w, unsigned h) : xDisplay(XOpenDisplay("")){
	if(!xDisplay){
		throw "表示先画面と接続できませんでした。";
	}


	//根窓設定
#ifdef TEST
	rootWindowID = XCreateSimpleWindow(
		xDisplay,
		RootWindow(xDisplay, 0),
		0, 0, w, h, 0,
		BlackPixel(xDisplay, 0), BlackPixel(xDisplay, 0));
	XMapWindow( xDisplay, rootWindowID );
#else
	rootWindowID = RootWindow(xDisplay, 0);
#endif

	//画面からの入力設定
	InputSetup();

	//OpenGLに諸条件を設定
	XVisualInfo *visual = glXChooseVisual(
		xDisplay, DefaultScreen(xDisplay), glxAttributes);
	glxContext = glXCreateContext(xDisplay, visual, NULL, True);
	XFree(visual);

	//この根窓の描画条件をカレントにする
	glXMakeCurrent(xDisplay, rootWindowID, glxContext);
}

//与えられたX鯖へ接続して根窓を取得してセットアップ
XDISPLAY::XDISPLAY(const char* server) : xDisplay(XOpenDisplay(server)){
	if(!xDisplay){
		throw "入力用画面と接続できませんでした。";
	}
	rootWindowID = RootWindow(xDisplay, 0);
}

XDISPLAY::~XDISPLAY(){
	XCloseDisplay(xDisplay);
}

void XDISPLAY::InputSetup(){
	printf("rootWindowID:%lu.\n", rootWindowID);

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

	//既存窓取得
	Window r;
	Window p;
	unsigned num;
	Window* wl;
	XQueryTree(xDisplay, rootWindowID, &r, &p, &wl, &num);
	if(!!wl){
		Window* w(wl);
		for(unsigned n(0); n < num; n++, w++){
			//TODO:XWINDOWを作ったらコメントアウト
//			XWINDOW::AtXCreate(xDisplay, *w, hCenter, vCenter);
		}
		XFree(wl);
	}
}


//エラーハンドラ
int XDISPLAY::damageBase;
int XDISPLAY::damage_err;
int XDISPLAY::XErrorHandler(Display* d, XErrorEvent* e){
	char description[256];
	XGetErrorText(d, (*e).error_code, description, 256);
	printf("%s(%u)\nserial:%lu\nreqCode:%u\nminCode:%u\n",
	       description,
	       (*e).error_code,
	       (*e).serial,
	       (*e).request_code,
	       (*e).minor_code);
	return 0;
}


















bool XDISPLAY::Run(){
	return false;
}


