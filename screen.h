#ifndef _SCREEN_
#define _SCREEN_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <GL/gl.h>
#include <GL/glx.h>

#include <stdio.h>

#include "matrix.h"
#include "list.h"
#include "window.h"
#include "room/room.h"
#include "rift.h"


class SCREEN{
	SCREEN();
	SCREEN(SCREEN&);
	void operator=(SCREEN&);
public:
	SCREEN(Display* const xDisplay, int xScreenIndex, FILE*, RIFT& rift);
	~SCREEN();
	/** drawListIndex(DisplayList)に格納された内容を再生 */
	static void Update(){
		for(SCREEN* s(list); s; s = (*s).next){
			(*s).Draw();
		}
	}
	static void AtPointed(XEvent&);
private:
	class VIEW{
	public:
		VIEW* next;
		VIEW(int left_, int top_, int width_, int height_, const GLfloat viewMatrix[]);
		void Draw(int listID);
	private:
		int left;
		int top;
		int right;
		int bottom;
		GLfloat viewMatrix[16];
	};
	static SCREEN* list;
	static const float defaultDotPitch = 0.003;
	static const float defaultDisplayDistance = 0.7;
	static const float nearDistance = 0.1;
	static const float farDistance = 10000;
	SCREEN* next;
	Display* const xDisplay;
	const int xScreenIndex;
	int width;
	int height;
	float realWidth;
	float realHeight;
	float realDistance;
	int xWindow;
	GLXContext glxContext;
	RIFT& rift;
	void _Draw(){
		glEnable(GL_DEPTH_TEST);
		WINDOW::Update(); //窓を描画
		glEnable(GL_LIGHTING);
		ROOM::Update(20);	//背景を描画
		glDisable(GL_LIGHTING);
	}
	void Draw(){
		glXMakeCurrent(xDisplay, xWindow, glxContext);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//描画準備
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		const float sizeRatio(nearDistance / (realDistance * 2));
		glFrustum(-realWidth * sizeRatio, realWidth * sizeRatio,
			-realHeight * sizeRatio, realHeight * sizeRatio,
			nearDistance, farDistance);

		//スクリーン共通マトリクスの読み込み
		glMatrixMode(GL_MODELVIEW);
		double rotation[16];
		rift.GetMatrix(rotation);
		glLoadMatrixd(rotation);
// 		glRotatef(180, 0, 1, 0);

		//描画を記録
// 		glNewList(xScreenIndex, GL_COMPILE);
// 		glEndList();

		//描画
// 		glEnable(GL_POLYGON_SMOOTH);
// 		glEnable(GL_BLEND);
// 		glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
		if(!rift.IsEnable()){
			glCallList(xScreenIndex);
		}else{
			const int inset(57);

			glViewport(inset, 0, width / 2, height);
			glPushMatrix();
			glTranslatef(0.03, 0, 0);
// 			glCallList(xScreenIndex);
			_Draw();
			glPopMatrix();

			glViewport(width / 2 - inset, 0, width / 2, height);
			glPushMatrix();
			glTranslatef(-0.03, 0, 0);
// 			glCallList(xScreenIndex);
			_Draw();
			glPopMatrix();

			//TODO:ここで描画バッファをテクスチャにして歪み付きで描画
		}

		//画面へ転送
		glXSwapBuffers(xDisplay, xWindow);
	};
};

#endif
