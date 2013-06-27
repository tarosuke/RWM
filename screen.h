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
	void _Draw();
	void PrepareDraw();
	void Draw(){
		glXMakeCurrent(xDisplay, xWindow, glxContext);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//描画
		glEnable(GL_POLYGON_SMOOTH);
// 		glEnable(GL_BLEND);
// 		glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
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
			glTranslatef(0.03, 0, 0);
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
			rift.GetView();
			glTranslatef(0.03, 0, 0);
// 			glScalef(0.5 , 1, 1);
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
			rift.GetView();
			glTranslatef(-0.03, 0, 0);
// 			glScalef(0.5 , 1, 1);
			_Draw();

			//TODO:ここで描画バッファをテクスチャにして歪み付きで描画
		}

		//画面へ転送
		glXSwapBuffers(xDisplay, xWindow);
	};
};

#endif
