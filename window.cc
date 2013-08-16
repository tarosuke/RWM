/******************************************************* window handler:window
 * -DTESTの時はrootになるウインドウを開いてそれをroot扱い。
 * でなければrootを取得してそれをrootとする。
 * rootに対してXCompositRedirectSubWindowsして内容を取得
 * また、窓に対してxdamegeイベントを受け付けるよう設定。
 */
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <X11/Xlib.h>

#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <window.h>
#include <view/view.h>
#include <toolbox/cyclic/cyclic.h>


Display* WINDOW::xDisplay;
unsigned WINDOW::rootWindowID;
GLXContext WINDOW::glxContext;
int WINDOW::rootWidth(1280);
int WINDOW::rootHeight(800);


//窓までの距離
float WINDOW::distance(0.6);
//窓の標準散開角(単位はOpenGLに合わせて°)
float WINDOW::horizAngle(160.0);
float WINDOW::vertAngle(80.0);

//窓リスト
TOOLBOX::QUEUE<WINDOW> WINDOW::windowList;


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
	GLX_STENCIL_SIZE, 8,
	GLX_ACCUM_RED_SIZE, 0,
	GLX_ACCUM_GREEN_SIZE, 0,
	GLX_ACCUM_BLUE_SIZE, 0,
	GLX_ACCUM_ALPHA_SIZE, 0,
	None
};


void WINDOW::Init(){
	xDisplay = XOpenDisplay(""),
#ifdef TEST
	rootWindowID = XCreateSimpleWindow(
		xDisplay,
		RootWindow(xDisplay, 0),
		0,
		0,
		rootWidth,
		rootHeight,
		0,
		BlackPixel(xDisplay, 0),
		BlackPixel(xDisplay, 0));
	XMapWindow( xDisplay, rootWindowID );
#else
	rootWindowID = RootWindow(xDisplay, 0);
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

	//OpenGLに諸条件を設定
	XVisualInfo *visual =
		glXChooseVisual(xDisplay, DefaultScreen(xDisplay), glxAttrs);
	glxContext = glXCreateContext(xDisplay, visual, NULL, True);
	XFree(visual);

	//設定した描画条件をカレントにする
	glXMakeCurrent(xDisplay, rootWindowID, glxContext);

	//部屋をたどる数をステンシルバッファの初期値に設定
	glClearStencil(VIEW::roomFollowDepth);
}

void WINDOW::Quit(){
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


void WINDOW::DrawAll(){
	//バッファのクリア
	glClear(GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);

	//基本設定
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	//基本ライト(場所は自分、明るさはAMBIENTへ)
	glEnable(GL_LIGHT0);
	glLightf(GL_LIGHT0, GL_POSITION, 0);
	const GLfloat myLight[] = { 0.3, 0.3, 0.3, 1 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, myLight);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0);

	//フォグ
// 	glFogi(GL_FOG_MODE, GL_LINEAR);
// 	glFogi(GL_FOG_START, 10);
// 	glFogi(GL_FOG_END, 5000);
// 	const GLfloat fogColor[] = { 0.6, 0.6, 0.6, 0.6 };
// 	glFogfv(GL_FOG_COLOR, fogColor);
// 	glHint(GL_FOG_HINT, GL_DONT_CARE);
// 	glEnable(GL_FOG);

	VIEW::DrawAll();
	glXSwapBuffers(xDisplay, rootWindowID);
}

void WINDOW::DrawWindows(){
	//窓描画(窓は陰影などなしでそのまま表示)
	glDisable(GL_LIGHTING);
	glDisable(GL_STENCIL_TEST);
	for(TOOLBOX::QUEUE<WINDOW>::ITOR i(windowList); i; i++){
		WINDOW& w(*i.Owner());
		if(w.mapped){
			w.Draw();
		}
	}
	glEnable(GL_LIGHTING);
}


void WINDOW::HandleXEvent(XEvent& e){
	XAnyEvent& ev(*(XAnyEvent*)&e);

	if(ev.type == MappingNotify){
		//MappingNotifyはev.windowが無効なので別処理
		AtMapping(e.xmapping);
		return;
	}

	switch(ev.type){
		case CreateNotify:
			AtCreate(e.xcreatewindow);
			break;
		case MapNotify:
			AtMap(e.xmap);
			break;
		case DestroyNotify:
			AtDestroy(e.xdestroywindow);
			break;
		case UnmapNotify:
			AtUnmap(e.xunmap);
			break;
		case KeyPress :
			static int window(-1);

			if(window < 0){
				window = XCreateSimpleWindow(
					xDisplay,
					rootWindowID,
					400,
					550,
					600,
					800,
					0,
					WhitePixel(xDisplay, 0),
					0x3C4048);
				XMapWindow( xDisplay, window);
			}else{
				XUnmapWindow(xDisplay, window);
				XDestroyWindow(xDisplay, window);
				window = -1;
			}
			break;
		default:
			//TODO:keyDown/Upの時などの処理を考える
			break;
	}
}

void WINDOW::Run(GHOST& user){
	DURATION duration;

	for(;;){
		while(XPending(xDisplay)){
			XEvent e;
			XNextEvent(xDisplay, &e);
			HandleXEvent(e);
		}
		VIEW::UpdateAll(duration.GetDuration());
		DrawAll();
	}
	Quit();
}


WINDOW* WINDOW::FindWindowByID(unsigned wID){
	for(TOOLBOX::QUEUE<WINDOW>::ITOR i(windowList); i; i++){
		WINDOW& w(*i.Owner());
		if(wID == w.wID){
			return &w;
		}
	}
	return 0;
}


void WINDOW::Draw(){
	glBindTexture(GL_TEXTURE_2D, tID);
	glPushMatrix();
	glRotatef(-horizAngle * horiz, 0, 1, 0);
	glRotatef(-vertAngle * vert, 1, 0, 0);
	glBegin(GL_TRIANGLE_STRIP);
	const float r(0.0002 * distance);
	const float w(r * width);
	const float h(r * height);
	//TODO:テクスチャ座標も付ける
	glTexCoord2f(0, 0);
	glVertex3f(-w, h, -distance);
	glTexCoord2f(0, 1);
	glVertex3f(-w, -h, -distance);
	glTexCoord2f(1, 0);
	glVertex3f(w, h, -distance);
	glTexCoord2f(1, 1);
	glVertex3f(w, -h, -distance);
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}

WINDOW::~WINDOW(){
	node.Detach();
	if(tID){
		glDeleteTextures(1, &tID);
	}
	if(wImage){
		XDestroyImage(wImage);
	}
}




void WINDOW::AtCreate(XCreateWindowEvent& e){
	if(e.window == rootWindowID){
		return;
	}
	WINDOW* const w(WINDOW::FindWindowByID(e.window));
	if(!w){
		//外部生成窓なので追随して生成
		WINDOW& nw = *new WINDOW(e.window);
		nw.horiz = ((float)e.x/rootWidth) - 0.5;
		nw.vert = ((float)e.y/rootHeight) - 0.5;
		nw.width = e.width;
		nw.height = e.height;

		//拡張イベントを設定
		XSelectInput (xDisplay, nw.wID, PropertyChangeMask);
		nw.dID = XDamageCreate(
			xDisplay, nw.wID, XDamageReportBoundingBox);
	}
}

void WINDOW::AtMap(XMapEvent& e){
	if(e.event != rootWindowID){
		return;
	}
	WINDOW* const w(WINDOW::FindWindowByID(e.window));
	if(w){
		//テクスチャ割り当て、初期画像設定
		(*w).AssignTexture();
		//map状態をXの窓に追随(trueなので以後Drawする)
		(*w).mapped = true;
	}
}
void WINDOW::AtDestroy(XDestroyWindowEvent& e){
	if(e.window == rootWindowID){
		return;
	}
	WINDOW* const w(WINDOW::FindWindowByID(e.window));
	if(w){
		//外部でDestroyされたウインドウに同期する
		delete w;
	}
}
void WINDOW::AtUnmap(XUnmapEvent& e){
	if(e.window == rootWindowID){
		return;
	}
	WINDOW* const w(WINDOW::FindWindowByID(e.window));
	if(w){
		//map状態をXの窓に追随(falseなので以後Drawしなくなる)
		(*w).mapped = false;
	}
}



void WINDOW::AssignTexture(){
fprintf(stderr, "assign...");
	//描画テスト
	GC gc(XCreateGC(xDisplay, wID, 0, 0));
	XSetForeground(xDisplay, gc, 0x00ff0000);
	XFillRectangle(xDisplay, wID, gc, 100, 10, 200, 400 );
	XSetForeground(xDisplay, gc, 0x000000ff);
	XFillArc(xDisplay, wID, gc, 300, 400, 400, 400, 0, 360 * 64);
	XSetForeground(xDisplay, gc, 0xD2DEF0);
	XSetFont(xDisplay, gc,
		 XLoadFont(xDisplay, "-*-*-*-*-*-*-24-*-*-*-*-*-iso8859-*"));
	const char* const str("First light!");
	XDrawString(xDisplay, wID, gc, 200, 600, str, strlen(str));

	XFreeGC(xDisplay, gc);

	//窓画像の取得
	if(wImage){
		XDestroyImage(wImage);
	}
fprintf(stderr, "createImage...");
#if 0
	const int w(1024);
	const int h(1024);
	wImage = XCreateImage(
		xDisplay,
		DefaultVisual(xDisplay,0),
		DefaultDepth(xDisplay,0),
		ZPixmap,0,0,w,h,32,0);
	(*wImage).data = (char*)malloc(w * h * 4);
	assert(wImage);
#if 0
	memset((*wImage).data, 0x40, w * h * 4);
#else
fprintf(stderr, "getImage...");
	XGetSubImage(
		xDisplay,
		wID,
		0, 0,
		width, height,
		AllPlanes,
		ZPixmap,
		wImage,
		0, 0);
#endif
#else
fprintf(stderr, "getImage...");
	const int w(width);
	const int h(height);
	wImage = XGetImage(
		xDisplay, wID, 0, 0, width, height, AllPlanes, ZPixmap);
#endif

	//テクスチャの生成
	if(!tID){
		glGenTextures(1, &tID);
	}
fprintf(stderr, "image2Texture(%u)...", tID);
	glBindTexture(GL_TEXTURE_2D, tID);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB, w, h, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, (*wImage).data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, 0);
fprintf(stderr, "OK.\n");
}

WINDOW::WINDOW(int wID) :
	wID(wID), node(*this), mapped(false), tID(0), wImage(0){
	windowList.Add(node);
}

