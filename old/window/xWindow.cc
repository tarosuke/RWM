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
	Display* d) :
	WINDOW(x, y, w, h),
	display(d),
	wID(window),
	xNode(*this){
	//X窓リストへ登録
	xWindowList.Insert(xNode);

	//XDamageイベントを受け取る設定
	XSelectInput(display, wID, PropertyChangeMask);
	dID = XDamageCreate(display, wID, XDamageReportNonEmpty);
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
void XWINDOW::AtXCreate(Display* const d, Window w, unsigned hc, unsigned vc){
	const XWINDOW* const xw(FindWindowByID(d, w));
	if(xw){
		//登録済なので終了
		return;
	}

	//新規窓登録
	XWindowAttributes attr;
	XGetWindowAttributes(d, w, &attr);
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
		display,
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
	XDamageSubtract(display, e.damage, None, None);

	//変化分をwImageへ取得
	XImage* const wImage(XGetImage(
		display,
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


void XWINDOW::AtXMouse(EVENT::EVENT_TYPE type, const XButtonEvent& xe){
	WINDOW::MOUSE_EVENT mouseEvent;
	mouseEvent.type = type;
	mouseEvent.x =
	mouseEvent.y = 0.0; //Rift上の座標には意味がない
	mouseEvent.hScroll =
	mouseEvent.vScroll = 0; //TODO:スクロールのサポート
	mouseEvent.buttonState = xe.button;
	mouseEvent.button =
	mouseEvent.clicks = 0;
	mouseEvent.orgEvent = static_cast<const void*>(&xe);
	WINDOW::AtMouse(mouseEvent);
}



void XWINDOW::OnKeyDown(const KEY_EVENT& e){
	XKeyEvent xe;
	xe.type = KeyPress;
	xe.display = display;
	xe.window = wID;
	xe.state =
		(e.modifiers & EVENT::ShiftKey ? ShiftMask : 0) |
		(e.modifiers & EVENT::CtrlKey ? ControlMask : 0) |
		(e.modifiers & EVENT::AltKey ? Mod1Mask : 0);
		xe.keycode = e.keyCode;
	xe.send_event = 0;
	XSendEvent(display, wID, true, 0, (XEvent*)&xe);
}
void XWINDOW::OnKeyUp(const KEY_EVENT& e){
	XKeyEvent xe;
	xe.type = KeyRelease;
	xe.display = display;
	xe.window = wID;
	xe.state =
	(e.modifiers & EVENT::ShiftKey ? ShiftMask : 0) |
	(e.modifiers & EVENT::CtrlKey ? ControlMask : 0) |
	(e.modifiers & EVENT::AltKey ? Mod1Mask : 0);
	xe.keycode = e.keyCode;
	xe.send_event = 0;
	XSendEvent(display, wID, true, 0, (XEvent*)&xe);
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
// 	printf("mouseDown:(%f %f %d %d) %08x.\n", e.x, e.y, vx, vy, e.buttonState);
	XButtonEvent xe;
	XButtonEvent& org(*(XButtonEvent*)e.orgEvent);
	xe.time = org.time;
	xe.serial = org.serial;
	xe.button = org.button;
	xe.type = ButtonPress;
	xe.window = wID;
	xe.display = display;
	xe.x = e.x;
	xe.y = e.y;
	xe.x_root = e.x + vx;
	xe.y_root = e.y + vy;
	xe.root = RootWindow(display, 0);
#if 1
	XSendEvent(const_cast<Display*>(display), wID, false, 0, (XEvent*)&xe);
#else
	SendEvent(wID, xe);
#endif
}

void XWINDOW::OnMouseUp(const MOUSE_EVENT& e){
// 	printf("mouseUp:(%f %f %d %d) %08x.\n", e.x, e.y, vx, vy, e.buttonState);
	XButtonEvent xe;
	XButtonEvent& org(*(XButtonEvent*)e.orgEvent);
	xe.time = org.time;
	xe.serial = org.serial;
	xe.button = org.button;
	xe.type = ButtonRelease;
	xe.window = wID;
	xe.display = display;
	xe.x = e.x;
	xe.y = e.y;
	xe.x_root = e.x + vx;
	xe.y_root = e.y + vy;
	xe.root = RootWindow(display, 0);
#if 1
	XSendEvent(display, wID, true, 0, (XEvent*)&xe);
#else
	SendEvent(wID, xe);
#endif
}

void XWINDOW::OnMouseMove(const MOUSE_EVENT& e){
// 	printf("mouseMove:(%f %f) %08x.\n", e.x, e.y, e.buttonState);
	XMotionEvent xe;
	xe.time = 0;
	xe.serial = 0;
	xe.type = MotionNotify;
	xe.window = wID;
	xe.display = display;
	xe.x = e.x;
	xe.y = e.y;
	xe.x_root = e.x + vx;
	xe.y_root = e.y + vy;
	xe.root = RootWindow(display, 0);
	XSendEvent(display, wID, true, 0, (XEvent*)&xe);
}


void XWINDOW::SendEvent(Window w, XButtonEvent& e){
	int x(e.x);
	int y(e.y);
	unsigned int numChildren;
	Window* children;
	if(!XQueryTree(display, w, 0, 0, &children, &numChildren) && children && numChildren){
		for(unsigned int n(0); n < numChildren; ++n){
			Window ww(children[numChildren - n - 1]);
			XWindowAttributes attr;
			XGetWindowAttributes(display, ww, &attr);

			//範囲内の子
			if(attr.x <= x && x < attr.x + attr.width &&
			   attr.y <= y && y < attr.y + attr.height){
				e.x -= attr.x;
				e.y -= attr.y;
				XFree(children);
				return SendEvent(ww, e);
			}
		}
	}
	if(children){
		XFree(children);
	}
	XSendEvent(e.display, w, false, 0, (XEvent*)&e);
}


