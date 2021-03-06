/***************************************************************** X eventloop
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "xDisplay.h"
#include <xWindow.h>



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
#else
	rootWindowID = RootWindow(xDisplay, 0);
#endif
printf("rootWindowID:%lu.\n", rootWindowID);

	//rootのサブウインドウをキャプチャ
	XCompositeRedirectSubwindows(
		xDisplay, rootWindowID, CompositeRedirectManual);

	//その他セットアップ
	Setup();
	SetupGL();
}

XDISPLAY::XDISPLAY(Display* d) : xDisplay(d){
	if(!xDisplay){
		//画面無効
		return;
	}
	rootWindowID = RootWindow(d, 0);
printf("fb:rootWindowID:%lu.\n", rootWindowID);

	//rootのサブウインドウをキャプチャ
#ifdef TEST
	XCompositeRedirectSubwindows(
		xDisplay, rootWindowID, CompositeRedirectAutomatic);
#else
	XCompositeRedirectSubwindows(
		xDisplay, rootWindowID, CompositeRedirectManual);
#endif
	//その他セットアップ
	Setup();
}

void XDISPLAY::Setup(){
	if(!xDisplay){
		//画面無効
		return;
	}
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

	//根窓中央を計算
	hCenter = width /2;
	vCenter = height /2;

	//既存窓取得
	Window r;
	Window p;
	unsigned num;
	Window* wl;
	XQueryTree(xDisplay, rootWindowID, &r, &p, &wl, &num);
	if(!!wl){
		Window* w(wl);
		for(unsigned n(0); n < num; n++, w++){
			XWINDOW::AtXCreate(xDisplay, *w, hCenter, vCenter);
		}
		XFree(wl);
	}
}

void XDISPLAY::SetupGL(){
	//OpenGLに諸条件を設定
	XVisualInfo *visual =
	glXChooseVisual(xDisplay, DefaultScreen(xDisplay), glxAttrs);
	glxContext = glXCreateContext(xDisplay, visual, NULL, True);
	XFree(visual);

	//この根窓の描画条件をカレントにする
	Activate();
}


XDISPLAY::~XDISPLAY(){
	if(!xDisplay){
		//画面無効
		return;
	}
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


void XDISPLAY::EventHandler(){
	if(!xDisplay){
		//画面無効
		return;
	}
	XEvent e;
	while(XPending(xDisplay)){
		XNextEvent(xDisplay, &e);
		switch(e.type){
		case CreateNotify:
			if(e.xcreatewindow.window != rootWindowID &&
				e.xcreatewindow.parent == rootWindowID){
				XWINDOW::AtXCreate(e.xcreatewindow, hCenter, vCenter);
			}
			break;
		case MapNotify:
			XWINDOW::AtXMap(e.xmap);
			break;
		case DestroyNotify:
			XWINDOW::AtXDestroy(e.xdestroywindow);
			break;
		case UnmapNotify:
			XWINDOW::AtXUnmap(e.xunmap);
			break;
		case KeyPress:
		case KeyRelease:
			XWINDOW::AtXKey(e.xkey);
			break;
		case ButtonPress:
			XWINDOW::AtXMouse(WINDOW::EVENT::mouseDown, e.xbutton);
			break;
		case ButtonRelease:
			XWINDOW::AtXMouse(WINDOW::EVENT::mouseUp, e.xbutton);
			break;
		case ConfigureNotify:
			XWINDOW::AtXConfigure(e.xconfigure);
			break;
#if 0
		case MappingNotify:
#if 1
			XRefreshKeyboardMapping(&e.xmapping);
#else
			WINDOW::AtMappingEvent(e.xmapping);
#endif
			break;
#endif
		default:
			if(e.type == damageBase + XDamageNotify){
				//XDamageのイベント
				XWINDOW::AtXDamage(e);
			}else{
				//その他のイベント(裏画面へ転送)
			}
			break;
		}
	}
}










/////////////////////////////////////////////////////////////////////// XVFB用

XVFB::XVFB() : XDISPLAY(StartFB()){
}

Display* XVFB::StartFB(){
	Display* d(XOpenDisplay(":15"));
	if(!d){
		perror("failed to open Xvfb.");
#if 0
		switch(fork()){
		case -1: //error
			break;
		case 0: //child
			//TODO:exec(ほげふが);
			break;
		default: //正常終了
			d = XOpenDisplay(":15");
			break;
		}
#endif
	}
	return d;
}


