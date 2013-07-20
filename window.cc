/******************************************************* window handler:window
 * -DTESTの時はrootになるウインドウを開いてそれをroot扱い。
 * でなければrootを取得してそれをrootとする。
 * rootに対してXCompositRedirectSubWindowsして内容を取得
 * また、窓に対してxdamegeイベントを受け付けるよう設定。
 */
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>

#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include <window.h>
#include <view.h>



WINDOW::ROOT WINDOW::root; //ルートウインドウのシングルトン


static int glxAttrs[] = {
	GLX_USE_GL,
	GLX_LEVEL, 0,
	GLX_RGBA,
	GLX_DOUBLEBUFFER,
	GLX_RED_SIZE, 8,
	GLX_GREEN_SIZE, 8,
	GLX_BLUE_SIZE, 8,
	GLX_ALPHA_SIZE, 8,
	GLX_DEPTH_SIZE, 24,
	GLX_STENCIL_SIZE, 0,
	GLX_ACCUM_RED_SIZE, 0,
	GLX_ACCUM_GREEN_SIZE, 0,
	GLX_ACCUM_BLUE_SIZE, 0,
	GLX_ACCUM_ALPHA_SIZE, 0,
	None
};


WINDOW::ROOT::ROOT() :
	xDisplay(XOpenDisplay("")),
#ifdef TEST
	rootWindowID(XCreateSimpleWindow(
		xDisplay,
		RootWindow(xDisplay, 0),
		0,
		0,
		width,
		height,
		0,
		BlackPixel(xDisplay, 0),
		BlackPixel(xDisplay, 0)))
#else
	rootWindowID(RootWindow(xDisplay, 0))
#endif
	{
#ifdef TEST
	XMapWindow( xDisplay, rootWindowID );
#endif
	//rootのサブウインドウをキャプチャ
	XCompositeRedirectSubwindows(
		xDisplay, rootWindowID, CompositeRedirectManual);

	//イベント選択
	XSelectInput(xDisplay, rootWindowID,
		SubstructureNotifyMask |
		StructureNotifyMask |
		PropertyChangeMask |
		ExposureMask |
		ButtonPressMask |
		ButtonReleaseMask |
		ButtonMotionMask |
		KeyPressMask |
		KeyReleaseMask);
	XFlush( xDisplay );
	XSync(xDisplay, false);

	XVisualInfo *visual =
		glXChooseVisual(xDisplay, DefaultScreen(xDisplay), glxAttrs);
	glxContext = glXCreateContext(xDisplay, visual, NULL, True);
	XFree(visual);

	glXMakeCurrent(xDisplay, rootWindowID, glxContext);
}

WINDOW::ROOT::~ROOT(){
	glXMakeCurrent(xDisplay, 0, NULL);
	glXDestroyContext(xDisplay, glxContext);

	XCompositeUnredirectSubwindows(
		xDisplay, rootWindowID, CompositeRedirectManual);

#ifdef TEST
	XUnmapWindow(xDisplay, rootWindowID);
	XDestroyWindow(xDisplay, rootWindowID);
#endif
	XSync(xDisplay, true);
for(;; sleep(1000));
	XCloseDisplay(xDisplay);
}


void WINDOW::ROOT::Draw(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//基本設定
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	//窓描画(窓は陰影などなしでそのまま表示)
	// 	glDisable(GL_LIGHTING);
	// 	WINDOW::Update(); //窓を描画
	glEnable(GL_LIGHTING);

	//基本ライト(場所は自分、明るさはAMBIENTへ)
	glEnable(GL_LIGHT0);
	glLightf(GL_LIGHT0, GL_POSITION, 0);
	const GLfloat myLight[] = { 0.3, 0.3, 0.3, 1 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, myLight);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0);

	//フォグ
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogi(GL_FOG_START, 10);
	glFogi(GL_FOG_END, 5000);
	const GLfloat fogColor[] = { 0.6, 0.6, 0.6, 0.6 };
	glFogfv(GL_FOG_COLOR, fogColor);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glEnable(GL_FOG);

	VIEW::DrawAll();
	glXSwapBuffers(xDisplay, rootWindowID);
}

void WINDOW::ROOT::Run(GHOST& user){
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	for(;;){
		while(XPending(xDisplay)){
			XEvent ev;
			XNextEvent(xDisplay, &ev);

			switch(ev.type){
				case CreateNotify:
					break;
				case MapNotify:
					break;
				case DestroyNotify:
				case UnmapNotify:
					break;
				case ReparentNotify:
					break;
				case MappingNotify:
					break;
				case MotionNotify:
					break;
				case ButtonPress:
				case ButtonRelease:
					break;
				case KeyPress:
					break;
				case KeyRelease:
					break;
				case ClientMessage:
					//その他の再描画リクエスト
					break;
				default:
					break;
			}
		}
		struct timespec t1;
		clock_gettime(CLOCK_MONOTONIC, &t1);
		const double f0(t.tv_sec + 0.000000001 * t.tv_nsec);
		const double f1(t1.tv_sec + 0.000000001 * t1.tv_nsec);
		const double d(f1 - f0);
		t = t1;
		VIEW::UpdateAll(d);
		Draw();
	}
}




