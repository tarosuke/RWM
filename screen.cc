#include <X11/extensions/Xcomposite.h>
#include <math.h>

#include <assert.h>
#include <stdio.h>

#include "screen.h"


SCREEN* SCREEN::list;

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
	GLX_STENCIL_SIZE, 0,
	GLX_ACCUM_RED_SIZE, 0,
	GLX_ACCUM_GREEN_SIZE, 0,
	GLX_ACCUM_BLUE_SIZE, 0,
	GLX_ACCUM_ALPHA_SIZE, 0,
	None
};


SCREEN::SCREEN(Display* const xDisplay_, int xScreenIndex, FILE* script) :
	next(0),
	xDisplay(xDisplay_),
	xScreenIndex(xScreenIndex),
#ifdef TEST
	width(800),
	height(600),
#else
	width(DisplayWidth(xDisplay_, xScreenIndex)),
	height(DisplayHeight(xDisplay_, xScreenIndex)),
#endif
	xWindow(XCreateSimpleWindow(
		xDisplay,
		RootWindow(xDisplay, 0),
		0,
		0,
		width,
		height,
		0,
		BlackPixel(xDisplay, 0),
		BlackPixel(xDisplay, 0))){
//	XCompositeUnredirectWindow(
//		xDisplay, xWindow, CompositeRedirectAutomatic);
	XMapWindow( xDisplay, xWindow );
	XFlush( xDisplay );
#ifndef TEST
	XCompositeRedirectSubwindows(
		xDisplay,
		XRootWindow(xDisplay, xScreenIndex),
		CompositeRedirectManual);
	XSync(xDisplay, false);
#endif
	XVisualInfo *visual = glXChooseVisual(xDisplay, xScreenIndex, glxAttrs);
	glxContext = glXCreateContext(xDisplay, visual, NULL, True);
	XFree(visual);

	//画面の実は位置を仮定(スクリプトで上書き予定)
	realWidth = defaultDotPitch * width;
	realHeight = defaultDotPitch * height;
	realDistance = defaultDisplayDistance;

	if(!script){
		//スクリプトを解釈してスクリーンを再配置
	}

	next = list;
	list = this;
};


SCREEN::~SCREEN(){
	glXMakeCurrent(xDisplay, 0, NULL);
	glXDestroyContext(xDisplay, glxContext);
}

/** イベント変換、回送
 * 1.evのwindowメンバから該当SCREENを探す
 * 2.SCREENの位置、向きに合わせて画面上の場所を角度へ変換
 * 3.WINDOWのAtPointedを呼ぶ
 */
void SCREEN::AtPointed(XEvent& ev){
}


