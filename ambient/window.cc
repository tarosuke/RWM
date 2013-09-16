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
#include <math.h>

#include <window.h>
#include <view/view.h>
#include <toolbox/cyclic/cyclic.h>


Display* WINDOW::xDisplay;
unsigned WINDOW::rootWindowID;
GLXContext WINDOW::glxContext;
int WINDOW::rootWidth(1280);
int WINDOW::rootHeight(800);
Atom WINDOW::wInstanceAtom;

//窓までの距離
float WINDOW::baseDistance(0.8);
//窓の標準散開角(1.0の位置。大きさはその弧の長さ。単位はOpenGLに合わせて°)
float WINDOW::spread(15.0);
float WINDOW::radSpread(spread * M_PI / 180); //そのラディアン(=長さ)

//窓リスト
TOOLBOX::QUEUE<WINDOW> WINDOW::windowList;

//XDamageイベントのベース値
int WINDOW::damageBase;
int WINDOW::damage_err;


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
	printf("rootWindowID:%d.\n", rootWindowID);
#else
	rootWindowID = RootWindow(xDisplay, 0);
#endif
	//rootのサブウインドウをキャプチャ
	XCompositeRedirectSubwindows(
		xDisplay, rootWindowID, CompositeRedirectManual);

	//エラーハンドラを設定
	XSetErrorHandler(XErrorHandler);

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

	//XDamageの設定
	XDamageQueryExtension(xDisplay, &damageBase, &damage_err);

	//WINDOWインスタンスへのポインタ格納のための設定
	wInstanceAtom = XInternAtom(xDisplay, "RWM_wInstance", false);
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
	unsigned nff(0);
	for(TOOLBOX::QUEUE<WINDOW>::ITOR i(windowList); i; i++){
		WINDOW& w(*i.Owner());
		if(w.mapped){
			w.Draw(nff++);
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
					0,
					0,
					600,
					800,
					0,
					WhitePixel(xDisplay, 0),
					0x3C4048);
				XMapWindow( xDisplay, window);
				//描画テスト
				GC gc(XCreateGC(xDisplay, window, 0, 0));
				XSetForeground(xDisplay, gc, 0x00ff0000);
				XFillRectangle(xDisplay, window, gc, 100, 10, 200, 400 );
				XSetForeground(xDisplay, gc, 0x000000ff);
				XFillArc(xDisplay, window, gc, 300, 400, 400, 400, 0, 360 * 64);
				XSetForeground(xDisplay, gc, 0xD2DEF0);
				XSetFont(xDisplay, gc,
					 XLoadFont(xDisplay, "-*-*-*-*-*-*-24-*-*-*-*-*-iso8859-*"));
				const char* const str("First light!");
				XDrawString(xDisplay, window, gc, 200, 600, str, strlen(str));

				XFreeGC(xDisplay, gc);
			}else{
				XUnmapWindow(xDisplay, window);
				XDestroyWindow(xDisplay, window);
				window = -1;
			}
			break;
		default:
			//XDamageのイベント
			if(ev.type == damageBase + XDamageNotify){
				AtDamage(*(XDamageNotifyEvent*)&e);
			}
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


void WINDOW::Draw(unsigned nff){
	const float distance(baseDistance + 0.03 * nff);
	glBindTexture(GL_TEXTURE_2D, tID);
	glPushMatrix();
	glRotatef(-horiz * spread, 0, 1, 0);
	glRotatef(-vert * spread, 1, 0, 0);
	glBegin(GL_TRIANGLE_STRIP);
	const float w(distance * hSpread);
	const float h(distance * vSpread);
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
}




void WINDOW::AtCreate(XCreateWindowEvent& e){
	if(e.window == rootWindowID){
		return;
	}
printf("create(%lu).\n", e.window);
	WINDOW* const w(WINDOW::FindWindowByID(e.window));
	if(!w){
		//追随して生成
		new WINDOW(e);
	}
}

void WINDOW::AtMap(XMapEvent& e){
	if(e.event != rootWindowID){
		return;
	}
printf("map(%lu).\n", e.event);
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
printf("destroy(%lu).\n", e.window);
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
printf("unmap(%lu).\n", e.window);
	WINDOW* const w(WINDOW::FindWindowByID(e.window));
	if(w){
		//map状態をXの窓に追随(falseなので以後Drawしなくなる)
		(*w).mapped = false;
	}
}
void WINDOW::AtDamage(XDamageNotifyEvent& e){
	if(e.drawable == rootWindowID){
		return;
	}
printf("damaged(%lu).\n", e.drawable);
	//変化分を取得したと通知
	XDamageSubtract(xDisplay, e.damage, None, None);

	//知っている窓なら変化分を反映
	WINDOW* const w(WINDOW::FindWindowByID(e.drawable));
	if(w){
		(*w).OnDamage(e);
	}
}

void WINDOW::OnDamage(XDamageNotifyEvent& e){
	if(!mapped){
		//非map状態では窓のキャプチャはできない
		return;
	}
printf("area:%d %d %d %d.\n", e.area.x, e.area.y, e.area.width, e.area.height);

	//変化分を取得してwImageを更新
	XImage* wImage(XGetImage(
		xDisplay,
		wID,
		e.area.x,
		e.area.y,
		e.area.width,
		e.area.height,
		AllPlanes,
		ZPixmap));

	//変化分をテクスチャに反映
	glBindTexture(GL_TEXTURE_2D, tID);
	glTexSubImage2D(
		GL_TEXTURE_2D, 0,
		e.area.x,
		e.area.y,
		e.area.width,
		e.area.height,
		GL_BGRA,
		GL_UNSIGNED_BYTE,
		(*wImage).data);
	glBindTexture(GL_TEXTURE_2D, 0);
	XDestroyImage(wImage);
}

void WINDOW::AssignTexture(){
	//窓画像の取得
	const int w(width);
	const int h(height);

	XImage* wImage(XGetImage(
		xDisplay, wID, 0, 0, width, height, AllPlanes, ZPixmap));
	assert(wImage && (*wImage).data);

	//テクスチャの生成
	if(!tID){
		glGenTextures(1, &tID);
	}
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
	XDestroyImage(wImage);
}

int WINDOW::XErrorHandler(Display* d, XErrorEvent* e){
	printf("(%u)\nserial:%lu\nreqCode:%u\nminCode:%u\n",
		(*e).error_code,
		(*e).serial,
		(*e).request_code,
		(*e).minor_code);
	return 0;
}

WINDOW::WINDOW(XCreateWindowEvent& e) :
	wID(e.window),
	node(*this),
	mapped(false),
	tID(0),
	width(e.width),
	height(e.height),
	hSpread(radSpread * width / rootWidth),
	vSpread(radSpread * height / rootWidth){
	windowList.Insert(node);

	if(!e.x && !e.y){
#if 0
		//空きを探索
		for(float l(0.0);; l += 1.0/(19 + l)){
			const float a(M_PI*2*l);
			const float h(0.1 * l * cos(a));
			const float v(0.1 * l * sin(a));
			const double d[3] = { v, h, 0.0 };
			const QON dir(d);
			WINDOW* w(FindWindowByDir(dir));
			if(!w){
				//重なってないので決定
				horiz = h;
				vert = v;
				break;
			}
		}
#else
		horiz = vert = 0.0;
#endif
	}else{
		if(windowReplaceOffset <= e.y){
			//既に移動した窓に合わせて生成された窓なので位置を戻す
			e.y -= windowReplaceOffset;
		}
		//位置指定があるのでそれに合わせる
		horiz = ((float)e.x/rootWidth) - 1.0;
		vert = ((float)e.y/rootHeight) - 1.0;
printf("%f %f.\n", horiz, vert);
	}

	//Xの管理上は見えない位置に移動
	XMoveWindow(xDisplay, e.window, 0, windowReplaceOffset);

	//拡張イベントを設定
	XSelectInput (xDisplay, wID, PropertyChangeMask);
	dID = XDamageCreate(
		xDisplay, wID, XDamageReportNonEmpty);
}

WINDOW* WINDOW::FindWindowByDir(const QON& dir){
	for(TOOLBOX::QUEUE<WINDOW>::ITOR i(windowList); i; i++){
		WINDOW& w(*i.Owner());
		//その方向にある窓を返す。窓は近い順なので最初に発見されたものを返す
		const P2 p(w.GetLocalPosition(dir));
		if(0.0 <= p.x && p.x < w.width &&
		   0.0 <= p.y && p.y < w.height){
			//dirが窓の中を指している
			return &w;
		}
	}
	return 0;
}

WINDOW::P2 WINDOW::GetLocalPosition(const QON&){
	P2 r;
	return r;
}

