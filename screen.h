#ifndef _SCREEN_
#define _SCREEN_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include <stdio.h>

#include "matrix.h"
#include "list.h"
#include "window.h"
#include "room.h"


class SCREEN{
	SCREEN();
	SCREEN(SCREEN&);
	void operator=(SCREEN&);
public:
	SCREEN(Display* const xDisplay, int xScreenIndex, FILE*);
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
		VIEW(int left, int top, int right, int bottom) :
			next(0),
			left(left),
			top(top),
			right(right),
			bottom(bottom){};
		VIEW* next;
		void Draw(int drawIndex){
			//視点の設定
			glViewport(left, top, right, bottom);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			//TODO:glRotateとかしてスクリーンの視点を設定
			gluPerspective(
				110,
				(float)(right - left) / (bottom - top),
				0.2,
				1000);

			//レンダリング
			glMatrixMode(GL_MODELVIEW);
			glCallList(drawIndex);
		};
	private:
		MATRIX<4> viewMatrix;
		int left;
		int top;
		int right;
		int bottom;
	};



	static SCREEN* list;
	SCREEN* next;
	Display* const xDisplay;
	const int xScreenIndex;
	int width;
	int height;
	int xWindow;
	GLXContext glxContext;
	VIEW* viewList;
	void Draw(){
		glXMakeCurrent(xDisplay, xWindow, glxContext);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//描画準備
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//描画を記録
		glNewList(xScreenIndex, GL_COMPILE);
		glEnable(GL_DEPTH_TEST);
		WINDOW::Update(); //窓を描画
		glEnable(GL_LIGHTING);
		ROOM::Update();	//背景を描画
		glDisable(GL_LIGHTING);
		glEndList();

		//描画
		for(VIEW* v(viewList); v; v = (*v).next){
			(*v).Draw(xScreenIndex);
		}

		//画面へ転送
		glXSwapBuffers(xDisplay, xWindow);
	};
};

#endif
