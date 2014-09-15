#include <pthread.h>

#include <X11/Xutil.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xcomposite.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <stdio.h>


#include "xDisplay.h"
#include "../window/event.h"
#include "xWindow.h"



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

	InputSetup();
}

XDISPLAY::~XDISPLAY(){
	XCloseDisplay(xDisplay);
}

void XDISPLAY::InputSetup(){
	printf("rootWindowID:%lu.\n", rootWindowID);

	modifyState = 0;

	//根窓の大きさ取得
	Window dw;
	int di;
	unsigned du;
	unsigned width;
	unsigned height;
	XGetGeometry(
		xDisplay,
		rootWindowID,
		&dw, &di, &di,
		&width, &height,
		&du, &du);
	hCenter = width / 2;
	vCenter = height / 2;

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
			XWINDOW::AtXCreate(xDisplay, *w, hCenter, vCenter);
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


void XDISPLAY::AtXKeyDown(const XKeyEvent& xe){
	const unsigned testState(ShiftMask | ControlMask);
	if((xe.state & testState) == testState && xe.keycode == 0x16){
		Quit();
		return;
	}
	KEY_DOWN_EVENT e;
	AtXKey(e, xe);
}
void XDISPLAY::AtXKeyUp(const XKeyEvent& xe){
	KEY_UP_EVENT e;
	AtXKey(e, xe);
}
void XDISPLAY::AtXKey(KEY_EVENT& e, const XKeyEvent& xe){
	//イベント変換
	e.modifiers = xe.state & ShiftMask ? EVENT::ShiftKey : 0;
	e.modifiers |= xe.state & ControlMask ? EVENT::CtrlKey : 0;
	e.modifiers |= xe.state & Mod1Mask ? EVENT::AltKey : 0;
	e.keyCode = xe.keycode;
	e.charCode = XLookupKeysym(
		const_cast<XKeyEvent*>(&xe),
		xe.state & ShiftMask ? 1 : 0);
	e.orgEvent = &xe;

	//イベント回送
	WINDOW::AtKey(e);
}

void XDISPLAY::AtXMouseDown(const XButtonEvent& xe){
	MOUSE_DOWN_EVENT e;
	AtXMouse(e, xe);
}
void XDISPLAY::AtXMouseUp(const XButtonEvent& xe){
	MOUSE_UP_EVENT e;
	AtXMouse(e, xe);
}
void XDISPLAY::AtXMouse(MOUSE_EVENT& e, const XButtonEvent& xe){
	e.x =
	e.y = 0.0; //Rift上の座標には意味がない
	e.hScroll =
	e.vScroll = 0; //TODO:スクロールのサポート
	e.buttonState = xe.button;
	e.button =
	e.clicks = 0;
	e.orgEvent = static_cast<const void*>(&xe);
	WINDOW::AtMouse(e);
}

void XDISPLAY::Run(){
	//Xのイベントを取得してEVENTを起こす
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
			AtXKeyDown(e.xkey);
			break;
		case KeyRelease:
			AtXKeyUp(e.xkey);
			break;
		case ButtonPress:
			AtXMouseDown(e.xbutton);
			break;
		case ButtonRelease:
			AtXMouseUp(e.xbutton);
			break;
		case ConfigureNotify:
			XWINDOW::AtXConfigure(e.xconfigure);
			break;
		case MappingNotify:
			XRefreshKeyboardMapping(&e.xmapping);
			//TODO:小窓への転送
			break;
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

void XDISPLAY::Update(){
	glXSwapBuffers(xDisplay, rootWindowID);
}


