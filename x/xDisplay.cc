#include <pthread.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xcomposite.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>


#include "xDisplay.h"




const int XDISPLAY::glxAttributes[] = {
	GLX_USE_GL,
	GLX_LEVEL, 0,
	GLX_RGBA,
	GLX_DOUBLEBUFFER,
	GLX_RED_SIZE, 8,
	GLX_GREEN_SIZE, 8,
	GLX_BLUE_SIZE, 8,
	GLX_ALPHA_SIZE, 8,
	GLX_DEPTH_SIZE, 24,
	GLX_STENCIL_SIZE, 8,
	GLX_ACCUM_RED_SIZE, 0,
	GLX_ACCUM_GREEN_SIZE, 0,
	GLX_ACCUM_BLUE_SIZE, 0,
	GLX_ACCUM_ALPHA_SIZE, 0,
	None
};


//TODO:根窓を取得してOpenGLとXDamageを含む各種イベントをセットアップ
XDISPLAY::XDISPLAY() : DISPLAY(Setup()){
}



//与えられたX鯖へ接続して寝窓を取得してセットアップ
XDISPLAY::XDISPLAY(const char* server) : DISPLAY(Setup()){
}




void XDISPLAY::Run(){
}



DISPLAY::PARAM XDISPLAY::Setup(){
	PARAM param = { 1280, 800 };
	return param;
}

