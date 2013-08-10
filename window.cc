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
#include <assert.h>

#include <window.h>
#include <view/view.h>
#include <toolbox/cyclic/cyclic.h>


Display* WINDOW::xDisplay;
unsigned WINDOW::rootWindowID;
GLXContext WINDOW::glxContext;
int WINDOW::rootWidth(1280);
int WINDOW::rootHeight(800);

//窓の標準散開角(単位はOpenGLに合わせて°)
float WINDOW::horizAngle(120.0);
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

	//窓描画(窓は陰影などなしでそのまま表示)
	glDisable(GL_LIGHTING);
	for(TOOLBOX::QUEUE<WINDOW>::ITOR i(windowList); i; i++){
		WINDOW& w(*i.Owner());
		if(w.mapped){
			w.Draw();
		}
	}
	glEnable(GL_LIGHTING);

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

void WINDOW::Run(GHOST& user){
	DURATION duration;
	for(;;){
		while(XPending(xDisplay)){
			XEvent e;
			XNextEvent(xDisplay, &e);
			XAnyEvent& ev(*(XAnyEvent*)&e);
			if(ev.display != xDisplay){
				//別のdisplayなので処理不要
				continue;
			}

			if(ev.type == MappingNotify){
				//MappingNotifyはev.windowが無効なので別処理
				AtMapping(e.xmapping);
				continue;
			}

			if(ev.window == rootWindowID){
				//根窓は表示されないので無視
				continue;
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
				default:
					//TODO:keyDown/Upの時などの処理を考える
					break;
			}
		}
		VIEW::UpdateAll(duration.GetDuration());
		DrawAll();
	}
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

}

WINDOW::~WINDOW(){
	node.Detach();
	//TODO:テクスチャの解放とか
}




void WINDOW::AtCreate(XCreateWindowEvent& e){
	WINDOW* const w(WINDOW::FindWindowByID(e.window));
	if(!w){
		//外部生成窓なので追随して生成
		WINDOW& nw = *new WINDOW(e.window);
		nw.horiz = ((float)(e.x)/(rootWidth - e.width)) - 0.5;
		nw.vert = ((float)(e.y)/(rootHeight - e.height)) - 0.5;
		nw.width = e.width;
		nw.height = e.height;
	}
}

void WINDOW::AtMap(XMapEvent& e){
	WINDOW* const w(WINDOW::FindWindowByID(e.window));
	if(w){
		//map状態をXの窓に追随(trueなので以後Drawする)
		(*w).mapped = true;
	}
}
void WINDOW::AtDestroy(XDestroyWindowEvent& e){
	WINDOW* const w(WINDOW::FindWindowByID(e.window));
	if(w){
		//外部でDestroyされたウインドウに同期する
		delete w;
	}
}
void WINDOW::AtUnmap(XUnmapEvent& e){
	WINDOW* const w(WINDOW::FindWindowByID(e.window));
	if(w){
		//map状態をXの窓に追随(falseなので以後Drawしなくなる)
		(*w).mapped = false;
	}
}





WINDOW::WINDOW(int wID) : wID(wID), node(*this), mapped(false){
	windowList.Add(node);
}

