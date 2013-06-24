#include <math.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

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


SCREEN::SCREEN(Display* const xDisplay_, int xScreenIndex, FILE* script, RIFT& rift_) :
	next(0),
	xDisplay(xDisplay_),
	xScreenIndex(xScreenIndex),
	width(rift.IsEnable() ? 1280 : DisplayWidth(xDisplay_, xScreenIndex)),
	height(rift.IsEnable() ? 800 : DisplayHeight(xDisplay_, xScreenIndex)),
	xWindow(XCreateSimpleWindow(
		xDisplay,
		RootWindow(xDisplay, 0),
		0,
		0,
		width,
		height,
		0,
		BlackPixel(xDisplay, 0),
		BlackPixel(xDisplay, 0))), rift(rift_){
//	XCompositeUnredirectWindow(
//		xDisplay, xWindow, CompositeRedirectAutomatic);
	XMapWindow( xDisplay, xWindow );
	XSelectInput(xDisplay, xWindow,
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

	XVisualInfo *visual = glXChooseVisual(xDisplay, xScreenIndex, glxAttrs);
	glxContext = glXCreateContext(xDisplay, visual, NULL, True);
	XFree(visual);

	//画面の実位置を仮定(スクリプトで上書き予定)
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
	XUnmapWindow(xDisplay, xWindow);
	XDestroyWindow(xDisplay, xWindow);
}


void SCREEN::_Draw(){
	glEnable(GL_DEPTH_TEST);
	WINDOW::Update(); //窓を描画
	glEnable(GL_LIGHTING);
	ROOM::Update(20);	//背景を描画
	glDisable(GL_LIGHTING);
}










/** イベント変換、回送
 * 1.evのwindowメンバから該当SCREENを探す
 * 2.SCREENの位置、向きに合わせて画面上の場所を角度へ変換
 * 3.WINDOWのAtPointedを呼ぶ
 */
void SCREEN::AtPointed(XEvent& ev){
}



















SCREEN::VIEW::VIEW(
	int left_,
	int top_,
	int width_,
	int height_,
	const GLfloat viewMatrix_[]) :
	left(left_),
	top(top_),
	right(left_ + width_),
	bottom(top_ + height_){
	memmove(viewMatrix, viewMatrix_, sizeof(viewMatrix));
}

void SCREEN::VIEW::Draw(int listID){
	glPushMatrix();
	glMultMatrixf(viewMatrix);
	glCallList(listID);
	glPopMatrix();
}
