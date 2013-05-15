#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/hiddev.h>
#include <sys/ioctl.h>
#include <string.h>
#include <poll.h>
#include <linux/hiddev.h>

#include "matrix.h"
#include "list.h"
#include "window.h"
#include "screen.h"
#include "rift.h"



class RWM{
public:
	RWM();
	void EventLoop();
private:
	Display* const xDisplay;
	WINDOW& FindWindow(XEvent&);

	static WINDOW nullWindow;

	void Update();
};
WINDOW RWM::nullWindow;


RWM::RWM() :
	xDisplay(XOpenDisplay("")){
	assert(xDisplay);

	static RIFT rift;

	//Screenの列挙
	XGrabServer(xDisplay);
	for(int i(0); i < ScreenCount(xDisplay); i++){
		new SCREEN(xDisplay, i, 0, rift);
	}
	XUngrabServer(xDisplay);

	//TODO:設定の読み込み
	//スクリプトによる背景のセットアップ
#ifdef TEST
	ROOM::Load(true);
#else
	ROOM::Load(false);
#endif
}

WINDOW& RWM::FindWindow(XEvent& xEvent){
	WINDOW* w(WINDOW::FindWindow(0,0));
	return w ? *w : nullWindow;
};

void RWM::EventLoop(){
	do{
		while(XPending(xDisplay)){
			XEvent xEvent;
			XNextEvent(xDisplay, &xEvent);

			switch(xEvent.type){
			case CreateNotify:
				new WINDOW(xEvent.xcreatewindow);
				break;
			case MapNotify:
				WINDOW::MapIt(xEvent.xmap);
				break;
			case DestroyNotify:
			case UnmapNotify:
				break;
			case ReparentNotify:
				break;
			case MappingNotify:
				break;
			case MotionNotify:
				SCREEN::AtPointed(xEvent);
				break;
			case ButtonPress:
			case ButtonRelease:
return;
			case KeyPress:
return;
			case KeyRelease:
				WINDOW::AtFocused(xEvent);
				break;
			case ClientMessage:
				//その他の再描画リクエスト
				break;
			default:
				break;
			}
		}
		Update();
	}while(1);
}


void RWM::Update(){
	SCREEN::Update();
}









int main(void){
	static RWM rwm;
	rwm.EventLoop();
	return 0;
}






#if 0
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>

#define W 640
#define H 400

static void draw(){
	glPushMatrix();
	glColor3f(0.5, 0.5, 1.0);
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(-0.5, -1, -1);
		glVertex3f(0.5, -1, -1);
		glVertex3f(-0.5, 1, -1);
		glVertex3f(0.5, 1, -1);
	glEnd();
	glPopMatrix();
}


static float distance = 0;
static void Eye(float offset){
	glPushMatrix();
	glTranslatef(offset, 0, -distance);
	glCallList(1);
	glPopMatrix();
}

static void disp(){
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	//左
	glViewport(0, 0, W/2, H);
	Eye(-0.1);

	//右
	glViewport(W/2, 0, W/2, H);
	Eye(0.1);

	glFlush();
}


static void timer(int val){
	static float v = 0.01;
	if((0.0 < v && 3.0 < distance) || (v < 0.0 && distance < 0)){
		v = - v;
	}
	distance += v;
	glutPostRedisplay();
	glutTimerFunc(20 , timer , 0);
}



int main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitWindowSize(W , H);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

	glutCreateWindow("Kitty on your lap");

	glNewList(1, GL_COMPILE);
	draw();
	glEndList();

	glutDisplayFunc(disp);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(110 , (float)W/H , 0.2 , 1000);
	glMatrixMode(GL_MODELVIEW);

	glutTimerFunc(20 , timer , 0);

	glutMainLoop();
	return 0;
}
#endif
