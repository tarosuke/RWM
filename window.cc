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


WINDOW::WINDOW(XCreateWindowEvent& xe) : xWindow(xe.window), mapped(false){};


void WINDOW::Map(XMapEvent& xe){
}




// WINDOW::POINT LocalCoords(WINDOW::POINT direction){
// 	POINT p;
// 	return p;
// }



void WINDOW::AtPointed(POINT& sight, XEvent& ev){};
void WINDOW::AtFocused(XEvent& ev){};


void WINDOW::Draw(){
	if(!mapped){
		return;
	}
	glPushMatrix();
	glColor3f(0.5, 0.5, 1.0);
	glRotatef(horiz, 0, 1, 0);
	glRotatef(virt, -1, 0, 0);
	glRectf(-width, -height,
		width, height);
	glTranslatef(0, 0, -distance);
	glScalef(0.5, 0.5, 0.5);
	glPopMatrix();
}
