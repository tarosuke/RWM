/******************************************************* window handler:window
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

#include <xWindow.h>
#include <xDisplay.h>
#include <view/view.h>


//窓全体関連
TOOLBOX::QUEUE<XWINDOW> XWINDOW::xWindowList;



XWINDOW::XWINDOW(
	float x,
	float y,
	int w,
	int h,
	Window window,
	const Display* d) :
	WINDOW(x, y, w, h),
	display(d),
	wID(window),
	xNode(*this){
	//X窓リストへ登録
	xWindowList.Insert(xNode);

	//XDamageイベントを受け取る設定
	XSelectInput(const_cast<Display*>(display), wID, PropertyChangeMask);
	dID = XDamageCreate(const_cast<Display*>(display), wID, XDamageReportNonEmpty);
}




//窓IDからXWINDOWのインスタンスを探す
XWINDOW* XWINDOW::FindWindowByID(const Display* d, Window w){
	for(TOOLBOX::QUEUE<XWINDOW>::ITOR i(xWindowList); i; i++){
		if(d == (*i).display && w == (*i).wID){
			return i;
		}
	}
	return 0;
}






///既存窓登録用
void XWINDOW::AtXCreate(const Display* d, Window w, unsigned hc, unsigned vc){
	const XWINDOW* const xw(FindWindowByID(d, w));
	if(xw){
		//登録済なので終了
		return;
	}

	//新規窓登録
	XWindowAttributes attr;
	XGetWindowAttributes(const_cast<Display*>(d), w, &attr);
	XWINDOW* const nxw(new XWINDOW(
		(float)attr.x - hc + attr.width / 2,
		(float)attr.y - vc + attr.height / 2,
		attr.width,
		attr.height, w, d));
	assert(nxw);
	(*nxw).vx = attr.x;
	(*nxw).vy = attr.y;
	(*nxw).xCenter = hc;
	(*nxw).yCenter = vc;

	//マップされていたらテクスチャ貼り付け
	if(IsUnmapped != attr.map_state){
		(*nxw).AssignXTexture();
	}

}

void XWINDOW::AtXCreate(const XCreateWindowEvent& e, unsigned hc, unsigned vc){
	const XWINDOW* const w(FindWindowByID(e.display, e.window));
	if(!w){
		//未登録窓ならインスタンス生成
		XWINDOW* const nxw(new XWINDOW(
			(float)e.x - hc + e.width / 2,
			(float)e.y - vc + e.height / 2,
			e.width, e.height,
			e.window, e.display));
		(*nxw).vx = e.x;
		(*nxw).vy = e.y;
		(*nxw).xCenter = hc;
		(*nxw).yCenter = vc;
	}
}

void XWINDOW::AtXDestroy(const XDestroyWindowEvent& e){
	XWINDOW* const xw(FindWindowByID(e.display, e.window));
	if(!xw){
		//そんな奴は知らん
		return;
	}
	delete xw;
}

void XWINDOW::AtXMap(const XMapEvent& e){
	XWINDOW* const xw(FindWindowByID(e.display, e.window));
	if(!xw){
		//そんな奴は知らん
		return;
	}
	(*xw).AssignXTexture();
	(*xw).SetVisibility(true);
}

void XWINDOW::AtXUnmap(const XUnmapEvent& e){
	XWINDOW* const xw(FindWindowByID(e.display, e.window));
	if(!xw){
		//そんな奴は知らん
		return;
	}
	(*xw).SetVisibility(false);
}


void XWINDOW::AssignXTexture(){
	//窓取得
	XImage* const wImage(XGetImage(
		const_cast<Display*>(display),
		wID,
		0, 0, width, height,
		AllPlanes, ZPixmap));
	if(wImage && (*wImage).data){
		//テクスチャの割り当て
		AssignImage((void*)(*wImage).data, width, height);
		XDestroyImage(wImage);
	}else{
		//取得できなかったけど後でdamageで送られてくるので割り当てておく
		void* dummyData(malloc(width * height * 4));
		AssignImage(dummyData, width, height);
		free(dummyData);
	}

	//窓を可視に設定
	SetVisibility(true);
}


void XWINDOW::AtXDamage(const XEvent& ev){
	XDamageNotifyEvent& e(*(XDamageNotifyEvent*)&ev);
	//知っている窓なら変化分を反映
	XWINDOW* const w(XWINDOW::FindWindowByID(e.display, e.drawable));
	if(w){
		(*w).OnDamage(e);
	}
}

void XWINDOW::OnDamage(XDamageNotifyEvent& e){
	//変化分を取得したと通知
	XDamageSubtract(const_cast<Display*>(display), e.damage, None, None);

	//変化分をwImageへ取得
	XImage* const wImage(XGetImage(
		const_cast<Display*>(display),
		wID,
		e.area.x,
		e.area.y,
		e.area.width,
		e.area.height,
		AllPlanes,
		ZPixmap));

	if(wImage){
		//変化分をテクスチャに反映
		UpdateImage(
			(*wImage).data,
			e.area.x,
			e.area.y,
			e.area.width,
			e.area.height);
		XDestroyImage(wImage);
	}else{
		puts("falied to capture image.");
	}
}



void XWINDOW::AtXKey(const XKeyEvent& e){
	const unsigned testState(ShiftMask | ControlMask);
	if(e.type == KeyRelease &&
	  (e.state & testState) == testState &&
	   e.keycode == 0x16){
		VIEW::Quit();
		return;
	}

	//イベント変換
	KEY_EVENT ev;
	ev.type = e.type == KeyPress ? EVENT::keyDown : EVENT::keyUp;
	ev.modifiers = e.state & ShiftMask ? EVENT::ShiftKey : 0;
	ev.modifiers |= e.state & ControlMask ? EVENT::CtrlKey : 0;
	ev.modifiers |= e.state & Mod1Mask ? EVENT::AltKey : 0;
	ev.keyCode = e.keycode;
	ev.charCode = XLookupKeysym(
		const_cast<XKeyEvent*>(&e),
		e.state & ShiftMask ? 1 : 0);

	//イベント回送
	AtKey(ev);
}


void XWINDOW::OnKeyDown(const KEY_EVENT& e){
	XKeyEvent xe;
	xe.type = KeyPress;
	xe.display = const_cast<Display*>(display);
	xe.window = wID;
	xe.state =
		(e.modifiers & EVENT::ShiftKey ? ShiftMask : 0) |
		(e.modifiers & EVENT::CtrlKey ? ControlMask : 0) |
		(e.modifiers & EVENT::AltKey ? Mod1Mask : 0);
		xe.keycode = e.keyCode;
	xe.send_event = 0;
	XSendEvent(const_cast<Display*>(display), wID, true, 0, (XEvent*)&xe);
}
void XWINDOW::OnKeyUp(const KEY_EVENT& e){
	XKeyEvent xe;
	xe.type = KeyRelease;
	xe.display = const_cast<Display*>(display);
	xe.window = wID;
	xe.state =
	(e.modifiers & EVENT::ShiftKey ? ShiftMask : 0) |
	(e.modifiers & EVENT::CtrlKey ? ControlMask : 0) |
	(e.modifiers & EVENT::AltKey ? Mod1Mask : 0);
	xe.keycode = e.keyCode;
	xe.send_event = 0;
	XSendEvent(const_cast<Display*>(display), wID, true, 0, (XEvent*)&xe);
}


void XWINDOW::AtXConfigure(const XConfigureEvent& e){
	XWINDOW* wp(FindWindowByID(e.display, e.window));
	if(wp){
		XWINDOW& w(*wp);
		if(w.vx != e.x || w.vy != e.y){
			//移動を検出
			w.OnMoved(e.x, e.y);
		}
		if(w.width != (unsigned)e.width ||
			w.height != (unsigned)e.height){
			//リサイズを検出
			w.OnResized(e.width, e.height);
		}
	}
}

void XWINDOW::OnMoved(int x, int y){
	vx = x;
	vy = y;
	Move((float)x - xCenter + width / 2, (float)y - yCenter + height / 2);
}

void XWINDOW::OnResized(unsigned w, unsigned h){
	Resize(w, h);
	if(IsVisible()){
		AssignXTexture();
	}
}




void XWINDOW::OnMouseDown(const MOUSE_EVENT& e){
	printf("mouseDown:(%f %f) %08x.\n", e.x, e.y, e.buttonState);
}

void XWINDOW::OnMouseUp(const MOUSE_EVENT& e){
	printf("mouseUp:(%f %f) %08x.\n", e.x, e.y, e.buttonState);
}

void XWINDOW::OnMouseEnter(const MOUSE_EVENT& e){
	printf("mouseEnter:(%f %f) %08x.\n", e.x, e.y, e.buttonState);
}

void XWINDOW::OnMouseLeave(const MOUSE_EVENT& e){
	printf("mouseLeave:(%f %f) %08x.\n", e.x, e.y, e.buttonState);
}











#if 0


void WINDOW::AtKeyEvent(XEvent& e){
	XKeyEvent& ke(e.xkey);
	const unsigned testState(ShiftMask | ControlMask);
	if(ke.type == KeyRelease &&
		(ke.state & testState) == testState){
		// 		  printf("keyCode:%x.\n", ke.keycode);
		if(ke.keycode == 0x16){
			VIEW::Quit();
		}
		}
		if(!!focused){
			WINDOW& w(*focused);
			if(w.mapped){
				//キーイベント回送
				Display* d(w.display.XDisplay());
				e.xkey.display = d;
				e.xkey.window = w.wID;
				e.xkey.subwindow = None;
				e.xkey.root = w.display.Root();;
				e.xkey.send_event = 1;
				XSendEvent(d, w.wID, true, 0, &e);

				//フォーカス窓を前面に移動
				windowList.Pick(w.node);
				return;
			}else{
				w.UnFocus();
			}
		}
}

void WINDOW::AtMappingEvent(XMappingEvent& e){
	puts("MappingNotify.");
	for(TOOLBOX::QUEUE<WINDOW>::ITOR i(windowList); i; i++){
		//全窓に配布
		WINDOW& w(*i);
		e.display = w.display.XDisplay();
		e.window = w.wID;
		XSendEvent(w.display.XDisplay(), w.wID, true, 0, (XEvent*)&e);
	}
}

int WINDOW::seenX;
int WINDOW::seenY;
void WINDOW::AtButtonEvent(XButtonEvent& e){
	if(!!focused){
		WINDOW& w(*focused);
		if(w.mapped){
			//ボダンイベント回送
			e.display = w.display.XDisplay();
			e.window = w.wID;
			e.root = RootWindow(w.display.XDisplay(), 0);
			e.x = seenX;
			e.y = seenY;
			e.x_root = w.vx + e.x;
			e.y_root = w.vy + e.y;
			e.send_event = 1;
			XSendEvent(w.display.XDisplay(), w.wID, true, 0, (XEvent*)&e);
		}
	}
}

void WINDOW::AtConfigureEvent(XConfigureEvent& e){
	WINDOW* wp(FindWindowByID(e.display, e.window));
	if(wp){
		WINDOW& w(*wp);
		if(w.vx != e.x || w.vy != e.y){
			//移動を検出
			w.Moved(e.x, e.y);
		}
		if(w.width != (unsigned)e.width ||
			w.height != (unsigned)e.height){
			//リサイズを検出
			w.Resized(e.width, e.height);
			}
	}
}


///// 個別イベントハンドラ

void WINDOW::OnDamage(XDamageNotifyEvent& e){
	if(!mapped){
		//非map状態では窓のキャプチャはできない
		return;
	}

	//変化分を取得したと通知
	XDamageSubtract(display.XDisplay(), e.damage, None, None);

	//変化分をwImageへ取得
	XImage* wImage(XGetImage(
		display.XDisplay(),
				 wID,
			  e.area.x,
			  e.area.y,
			  e.area.width,
			  e.area.height,
			  AllPlanes,
			  ZPixmap));

	if(wImage){
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
	}else{
		puts("falied to capture image.");
	}
}

void WINDOW::AssignTexture(){
	//窓画像の取得
	const int w(width);
	const int h(height);

	XImage* wImage(XGetImage(
		display.XDisplay(), wID, 0, 0, width, height, AllPlanes, ZPixmap));

	//テクスチャの生成
	if(!tID){
		glGenTextures(1, &tID);
	}
	glBindTexture(GL_TEXTURE_2D, tID);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	if(wImage && (*wImage).data){
		//正常に取得
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB, w, h, 0,
	       GL_BGRA, GL_UNSIGNED_BYTE, (*wImage).data);
		XDestroyImage(wImage);
	}else{
		//取得できなかったけど割り当てておく
		void* dummyData(malloc(w * h * 4));
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB, w, h, 0,
	       GL_BGRA, GL_UNSIGNED_BYTE, dummyData);
		free(dummyData);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, 0);
}


WINDOW::WINDOW(XCreateWindowEvent& e, XDISPLAY& xDisplay, bool mapState) :
display(xDisplay),
wID(e.window),
node(*this),
mapped(mapState),
tID(0),
vx(e.x),
vy(e.y),
width(e.width),
height(e.height){
	const unsigned rootWidth(display.Width());
	const unsigned rootHeight(display.Height());

	//窓リストへ登録
	windowList.Insert(node);

	//幅と高さ(rw=π=180°とした時の半径baseDistance上の弧の長さ)
	//位置の時は弧の長さ、大きさの時は(弦の長さではなくただの)長さとして使う
	//ピクセルサイズを乗じると空間中の長さになる値
	scale = baseDistance * M_PI / rootWidth;

	if(!vx && !vy && width != rootWidth && height != rootHeight){
		//未指定なので最適な場所を探索して移動
		//TODO:画面をグリッド状に走査しgravity位置からの距離及び窓の重なり面積をポイントとしてポイントが最も小さい位置を採用する。重なり面積は重なっているかどうかではなく重なっている窓それぞれについて加算する。
		const int gx(rootWidth * 0.5);
		const int gy(rootHeight * 0.5);

		//場所の絞り込み
		SeekPosition(
			rootWidth - width,
	       rootHeight - height,
	       256, 256, gx, gy);
		SeekPosition(vx + 256, vy + 256, 16, 16, gx, gy);
		SeekPosition(vx + 16, vy + 16, 1, 1, gx, gy);
	}

	//窓位置に合わせて仮想空間内の向きを決める
	Move(vx, vy);

	//拡張イベントを設定
	XSelectInput(display.XDisplay(), wID, PropertyChangeMask);
	dID = XDamageCreate(
		display.XDisplay(), wID, XDamageReportNonEmpty);

	//mapStateが真ならテクスチャを割り当てておく
	if(mapState){
		AssignTexture();
	}
}

void WINDOW::SeekPosition(
	int hTo, int vTo,
	int hStep, int vStep,
	int gx, int gy){
	int tx(vx);
	int ty(vy);
	unsigned pt(~0U);
	for(int y(vy); y < vTo; y += vStep){
		for(int x(vx); x < hTo; x += hStep){
			const unsigned p(
				WindowPositionPoint(x, y, gx, gy));
			if(p < pt){
				//最小ペナルティが出たので値を差し替える
				pt = p;
				tx = x;
				ty = y;
			}
		}
	}
	vx = tx;
	vy = ty;
	}

	unsigned WINDOW::OverLen(int s0, int l0, int s1, int l1){
		const int e0(s0 + l0);
		const int e1(s1 + l1);
		//重なりの長さを求める
		if(s0 <= s1){
			if(s1 < e0){
				return (e0 < e1 ? e0 : e1) - s1;
			}
		}else{
			if(s0 < e1){
				return (e1 < e0 ? e1 : e0) - s0;
			}
		}
		return 0;
	}

	unsigned WINDOW::WindowPositionPoint(int x, int y, int gx, int gy){
		//重力ペナルティ
		const int dx(gx - (x + width / 2));
		const int dy(gy - (y + height / 2));
		unsigned p(dx*dx + dy*dy);

		//重なりペナルティ
		for(TOOLBOX::QUEUE<WINDOW>::ITOR i(windowList); i; i++){
			const WINDOW& w(*i);
			if(&w == this){
				//自分自身は除外
				continue;
			}
			const unsigned hl(OverLen(x, width, w.vx, w.width));
			const unsigned vl(OverLen(y, height, w.vy, w.height));
			const unsigned p1(hl * vl);
			p += p1 * p1;
		}
		return p;
	}

	WINDOW::P2 WINDOW::GetLocalPosition(const COMPLEX<4>& d){
		COMPLEX<4> dir(d);

		//頭の向きをベクタにして窓の中心を基準に変換
		VECTOR<3> tgt((const double[]){ 0, 0, 1});
		tgt.ReverseRotate(center);
		tgt.ReverseRotate(dir);

		//窓の上の位置に直す
		const double* a(tgt);
		const float s(distance / (scale * a[2]));
		P2 r = { (float)(a[0] * s) + width * 0.5f,
			(float)(-a[1] * s) + height * 0.5f };
			return r;
	}

	void WINDOW::Moved(int x, int y){
		const unsigned rootWidth(display.Width());
		const unsigned rootHeight(display.Height());
		horiz = ((x + width*0.5) - (float)rootWidth/2.0) * scale;
		vert = ((y + height*0.5) - (float)rootHeight/2.0) * scale;

		//四元数表現
		COMPLEX<4>::ROTATION v = { vert, { 1, 0, 0 } };
		COMPLEX<4>::ROTATION h = { horiz, { 0, 1, 0 } };
		COMPLEX<4> hq(h);
		COMPLEX<4> vq(v);
		hq *= vq;
		center = hq;

		//フレームバッファ上表現
		vx = x;
		vy = y;
		printf("%lu:%d %d(%d %d).\n", wID, x, y, width, height);
	}

	void WINDOW::Move(int x, int y){
		//仮想空間側の窓を移動
		Moved(x, y);

		//それがXの窓ならフレームバッファ側の窓を移動
		if(wID){
			XMoveWindow(display.XDisplay(), wID, x, y);
		}
	}


	void WINDOW::Focus(){
		if(focused != this){
			XSetInputFocus(display.XDisplay(), wID, RevertToParent, CurrentTime);
			focused = this;
		}
	}

	void WINDOW::UnFocus(){
		if(focused == this){
			focused = 0;
		}
	}


	void WINDOW::See(int x, int y){
		//視線カーソル移動
		XWarpPointer(display.XDisplay(),
			     None,
	       wID,
	       0, 0,
	       0, 0,
	       x, y);
		seenX = x;
		seenY = y;
	}

	void WINDOW::Resized(unsigned w, unsigned h){
		width = w;
		height = h;

		//テクスチャの入れ替え
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &tID);
		tID = 0;
		AssignTexture();
	}

	void WINDOW::Resize(unsigned w, unsigned h){
		//X側の窓をリサイズ(仮想空間側はConfigureNotifyで追随)
		XResizeWindow(display.XDisplay(), wID, width, height);
	}



#endif
