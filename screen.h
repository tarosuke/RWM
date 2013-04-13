#ifndef _SCREEN_
#define _SCREEN_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <GL/gl.h>
#include <GL/glx.h>

#include "matrix.h"
#include "list.h"


class SCREEN{
	SCREEN();
	SCREEN(SCREEN&);
	void operator=(SCREEN&);
public:
	SCREEN(Display* const xDisplay, int xScreenIndex);
	~SCREEN();
	class ITOR{
	public:
		ITOR() : nowOn(SCREEN::list){};
		operator SCREEN&(){
			return *nowOn;
		};
		SCREEN& operator++(int){
			if(nowOn){
				nowOn = (*nowOn).next;
			}else{
				throw;
			}
			return *nowOn;
		};
	private:
		SCREEN* nowOn;
	};
	static void AtPointed(XEvent&);
private:
	static SCREEN* list;
	SCREEN* next;
	Display* const xDisplay;
	int width;
	int height;
	int xWindow;
	GLXContext glxContext;
	MATRIX<4> viewMatrix;
};

#endif
