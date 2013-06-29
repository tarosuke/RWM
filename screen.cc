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
	//自分の位置(仮)
	glTranslatef(0.5, -1.0, 0);

	//基本設定
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	//窓描画(窓は陰影などなしでそのまま表示)
	glDisable(GL_LIGHTING);
	WINDOW::Update(); //窓を描画
	glEnable(GL_LIGHTING);

	//基本ライト(場所は自分、明るさはAMBIENTへ)
	glEnable(GL_LIGHT0);
	glLightf(GL_LIGHT0, GL_POSITION, 0);
	const GLfloat myLight[] = { 0.3, 0.3, 0.3, 1 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, myLight);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0);

	//フォグ
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogi(GL_FOG_START, 10);
	glFogi(GL_FOG_END, 5000);
	const GLfloat fogColor[] = { 0.6, 0.6, 0.6, 0.6 };
	glFogfv(GL_FOG_COLOR, fogColor);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glEnable(GL_FOG);

	ROOM::Update(20);	//背景を描画
}

void SCREEN::Draw(){
	glXMakeCurrent(xDisplay, xWindow, glxContext);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//描画
	const float sr(nearDistance / (realDistance * 2));
	if(!rift.IsEnable()){
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-realWidth * sr, realWidth * sr,
			  -realHeight * sr, realHeight * sr,
	    nearDistance, farDistance);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		rift.GetView();
		glScalef(0.5 , 1, 1);
		_Draw();
	}else{
		const int hw(width / 2);
		const int rhw(realWidth / 2);
		const double inset(0.17);

		//左目
		glViewport(0, 0, hw, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum((-rhw - inset) * sr, (rhw - inset) * sr,
			  -realHeight * sr, realHeight * sr,
	    nearDistance, farDistance);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.03, 0, 0);
		rift.GetView();
		_Draw();

		//右目
		glViewport(hw, 0, hw, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum((-rhw + inset) * sr, (rhw + inset) * sr,
			  -realHeight * sr, realHeight * sr,
	    nearDistance, farDistance);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(-0.03, 0, 0);
		rift.GetView();
		_Draw();

		//TODO:ここで描画バッファをテクスチャにして歪み付きで描画
	}

	//画面へ転送
	glXSwapBuffers(xDisplay, xWindow);
}









/** イベント変換、回送
 * 1.evのwindowメンバから該当SCREENを探す
 * 2.SCREENの位置、向きに合わせて画面上の場所を角度へ変換
 * 3.WINDOWのAtPointedを呼ぶ
 */
void SCREEN::AtPointed(XEvent& ev){
}


