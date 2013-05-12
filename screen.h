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
	void RotateR(const float angle){
		Rotate(angle, 0, 0, -1);
	};
	void RotateP(const float angle){
		Rotate(angle, -1, 0, 0);
	};
	void RotateY(const float angle){
		Rotate(angle, 0, 1, 0);
	};
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
	GLfloat matrix[16];
	void Rotate(const float angle, const float x, const float y, const float z);
	void Draw(){
		glXMakeCurrent(xDisplay, xWindow, glxContext);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//描画準備
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		const float sizeRatio(nearDistance / (realDistance * 2));
		glFrustum(-realWidth * sizeRatio, realWidth * sizeRatio,
			-realHeight * sizeRatio, realHeight * sizeRatio,
			nearDistance, farDistance);

		//スクリーン共通マトリクスの読み込み
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(matrix);

		//描画を記録
		glNewList(xScreenIndex, GL_COMPILE);
		glEnable(GL_DEPTH_TEST);
		WINDOW::Update(); //窓を描画
		glEnable(GL_LIGHTING);
		ROOM::Update(20);	//背景を描画
		glDisable(GL_LIGHTING);
		glEndList();

		//描画
		glCallList(xScreenIndex);

		//画面へ転送
		glXSwapBuffers(xDisplay, xWindow);
	};
};

#endif
