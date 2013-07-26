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


TOOLBOX::QUEUE<WINDOW> WINDOW::windowList; //窓リスト


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
	DURATION duration;
	for(;;){
		while(XPending(xDisplay)){
			XEvent ev;
			XNextEvent(xDisplay, &ev);

			switch(ev.type){
				case CreateNotify:
					AtCreate(ev.xcreatewindow);
					break;
				case MapNotify:
					AtMap(ev.xmap);
					break;
				case DestroyNotify:
					AtDestroy(ev.xdestroywindow);
					break;
				case UnmapNotify:
					AtUnmap(ev.xunmap);
					break;
				default:
					//TODO:keyDown/Upの時などの処理を考える
					break;
			}
		}
		VIEW::UpdateAll(duration.GetDuration());
		Draw();
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

void WINDOW::DrawAll(){
	for(TOOLBOX::QUEUE<WINDOW>::ITOR i(windowList); i; i++){
		WINDOW& w(*i.Owner());
		if(w.mapped){
		}
	}
}

WINDOW::~WINDOW(){
	node.Detach();
// 	XUnmapWindow(room.wID);
// 	XDestroyWindow(wID);
	//TODO:テクスチャの解放とか
}




void WINDOW::ROOT::AtCreate(XCreateWindowEvent& e){
	if(rootWindowID == e.window){
		//ルートなので処理不要
		return;
	}
	WINDOW* const w(WINDOW::FindWindowByID(e.window));
	if(!w){
		//外部生成窓なので追随して生成
		new WINDOW(e.window);
	}
}

void WINDOW::ROOT::AtMap(XMapEvent& e){
	if(rootWindowID == e.window){
		//ルートなので処理不要
		return;
	}
	WINDOW* const w(WINDOW::FindWindowByID(e.window));
	assert(w);
	//map状態を変更(trueなので以後Drawする)
	(*w).mapped = true;
}
void WINDOW::ROOT::AtDestroy(XDestroyWindowEvent& e){
	if(rootWindowID == e.window){
		//ルートなので処理不要
		return;
	}
	WINDOW* const w(WINDOW::FindWindowByID(e.window));
	assert(w);
	//ウインドウを消去
	delete w;
}
void WINDOW::ROOT::AtUnmap(XUnmapEvent& e){
	if(rootWindowID == e.window){
		//ルートなので処理不要
		return;
	}
	WINDOW* const w(WINDOW::FindWindowByID(e.window));
	assert(w);
	//map状態を変更(falseなので以後Drawしなくなる)
	(*w).mapped = false;
}







WINDOW::WINDOW(int wID) : wID(wID), mapped(false), node(*this){
	windowList.Add(node);
}

