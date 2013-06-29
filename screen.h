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
#include "room/texture.h"
#include "view.h"



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
// 	VIEW view;
//  	TEXTURE texture;
	void _Draw();
	void Draw();
};

#endif
