#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include <assert.h>
#include <stdio.h>

#include "matrix.h"
#include "list.h"
#include "window.h"


LIST<WINDOW> WINDOW::list;

WINDOW* WINDOW::FindWindow(float h, float v){
	return 0;
}


WINDOW::WINDOW(int xWindow_) : xWindow(xWindow_){};


// WINDOW::POINT LocalCoords(WINDOW::POINT direction){
// 	POINT p;
// 	return p;
// }



void WINDOW::AtPointed(POINT& sight, XEvent& ev){};
void WINDOW::AtFocused(XEvent& ev){};


void WINDOW::Update(){
	for(LIST<WINDOW>::ITOR i(list); i; i++){
		(*i).Draw();
	}
}
