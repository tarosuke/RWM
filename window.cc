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



//窓までの距離
float WINDOW::baseDistance(0.6);

//窓全体関連
TOOLBOX::QUEUE<WINDOW> WINDOW::windowList;
WINDOW* WINDOW::focused(0);



//窓IDからWINDOWのインスタンスを探す
WINDOW* WINDOW::FindWindowByID(Display* display, unsigned wID){
	for(TOOLBOX::QUEUE<WINDOW>::ITOR i(windowList); i; i++){
		WINDOW& w(*i.Owner());
		if(display == w.xDisplay && wID == w.wID){
			return &w;
		}
	}
	return 0;
}


/////窓描画関連
bool WINDOW::zoomable(false);
const QON* WINDOW::headDir;
const float WINDOW::zoomedScale(0.001);
void WINDOW::DrawAll(const QON& dir){
	zoomable = true;
	headDir = &dir;
	unsigned n(0);
	for(TOOLBOX::QUEUE<WINDOW>::ITOR i(windowList); i; i++){
		if((*i).mapped){
			(*i).Draw(n++);
		}
	}
}

void WINDOW::Draw(unsigned nff){
	//ズーム処理
	float s(scale);
	P2 offset = { 0, 0 };
	if(zoomable){
		const P2 center = GetLocalPosition(*headDir);
		if(0 <= center.x && center.x < width &&
		   0 <= center.y && center.y < height){
			//フォーカス取得
			Focus();

			//ズーム時パラメタ設定
			zoomable = false;
			if(scale < zoomedScale){
				//拡大率設定
				s = zoomedScale;

				//オフセット算出
				const float zr(zoomedScale - scale);
				const float hw(width * 0.5f);
				const float hh(height * 0.5f);
				const float dh(hw - center.x);
				const float dv(center.y - hh);
				offset.x = dh * zr;
				offset.y = dv * zr;
			}
		}
	}

	//窓までの距離
	distance = baseDistance + 0.03 * nff;

	//窓描画
	glPushMatrix();
	glRotatef(-horiz * 180 / M_PI, 0, 1, 0);
	glRotatef(-vert * 180 / M_PI, 1, 0, 0);

	glBindTexture(GL_TEXTURE_2D, tID);
	glBegin(GL_TRIANGLE_STRIP);
	const float w(s * width * 0.5);
	const float h(s * height * 0.5);
	const float l(-w + offset.x);
	const float r(w + offset.x);
	const float t(-h + offset.y);
	const float b(h + offset.y);
	glTexCoord2f(0, 0);
	glVertex3f(l, b, -distance);
	glTexCoord2f(0, 1);
	glVertex3f(l, t, -distance);
	glTexCoord2f(1, 0);
	glVertex3f(r, b, -distance);
	glTexCoord2f(1, 1);
	glVertex3f(r, t, -distance);
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}

WINDOW::~WINDOW(){
	UnFocus();
	node.Detach();
	if(tID){
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &tID);
	}
}



//////イベント処理関連
void WINDOW::AtCreate(XCreateWindowEvent& e, unsigned rw, unsigned rh){
	WINDOW* const w(WINDOW::FindWindowByID(e.display, e.window));
	if(!w){
		//追随して生成
		new WINDOW(e, rw, rh);
	}
}

void WINDOW::AtMap(XMapEvent& e){
	WINDOW* const w(WINDOW::FindWindowByID(e.display, e.window));
	if(w){
		//テクスチャ割り当て、初期画像設定
		(*w).AssignTexture();
		//map状態をXの窓に追随(trueなので以後Drawする)
		(*w).mapped = true;
	}
}
void WINDOW::AtDestroy(XDestroyWindowEvent& e){
	WINDOW* const w(WINDOW::FindWindowByID(e.display, e.window));
	if(w){
		//外部でDestroyされたウインドウに同期する
		delete w;
	}
}
void WINDOW::AtUnmap(XUnmapEvent& e){
	WINDOW* const w(WINDOW::FindWindowByID(e.display, e.window));
	if(w){
		//map状態をXの窓に追随(falseなので以後Drawしなくなる)
		if(focused == w){
			focused = w;
		}
		(*w).mapped = false;
	}
}
void WINDOW::AtDamage(XEvent& ev){
	XDamageNotifyEvent& e(*(XDamageNotifyEvent*)&ev);
	//知っている窓なら変化分を反映
	WINDOW* const w(WINDOW::FindWindowByID(e.display, e.drawable));
	if(w){
		(*w).OnDamage(e);
	}
}

void WINDOW::AtKeyEvent(XEvent& e){
	if(!!focused){
		WINDOW& w(*focused);
		if(w.mapped){
			//キーイベント回送
			e.xkey.display = w.xDisplay;
			e.xkey.window = w.wID;
			e.xkey.subwindow = None;
			e.xkey.root = RootWindow(w.xDisplay, 0);
			e.xkey.send_event = 1;
			XSendEvent(w.xDisplay, w.wID, true, 0, &e);

			//フォーカス窓を前面に移動
			windowList.Pick(w.node);
			return;
		}else{
			w.UnFocus();
		}
	}
}

void WINDOW::AtButtonEvent(XEvent& e){
}



///// 個別イベントハンドラ

void WINDOW::OnDamage(XDamageNotifyEvent& e){
	if(!mapped){
		//非map状態では窓のキャプチャはできない
		return;
	}

	//変化分を取得したと通知
	XDamageSubtract(xDisplay, e.damage, None, None);

	//変化分をwImageへ取得
	XImage* wImage(XGetImage(
		xDisplay,
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
		xDisplay, wID, 0, 0, width, height, AllPlanes, ZPixmap));

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


WINDOW::WINDOW(XCreateWindowEvent& e, unsigned rw, unsigned rh) :
	xDisplay(e.display),
	wID(e.window),
	node(*this),
	mapped(false),
	tID(0),
	vx(e.x),
	vy(e.y),
	width(e.width),
	height(e.height),
	rootWidth(rw),
	rootHeight(rh){

	//窓リストへ登録
	windowList.Insert(node);

	//幅と高さ(rw=π=180°とした時の半径baseDistance上の弧の長さ)
	//位置の時は弧の長さ、大きさの時は(弦の長さではなくただの)長さとして使う
	//ピクセルサイズを乗じると空間中の長さになる値
	scale = baseDistance * M_PI / rw;

	if(!vx && !vy && width != rootWidth && height != rootHeight){
		//未指定なので最適な場所を探索して移動
		//TODO:画面をグリッド状に走査しgravity位置からの距離及び窓の重なり面積をポイントとしてポイントが最も小さい位置を採用する。重なり面積は重なっているかどうかではなく重なっている窓それぞれについて加算する。
		const int gx((rootWidth - width) / 2);
		const int gy((rootHeight - height) / 2);

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
	XSelectInput(xDisplay, wID, PropertyChangeMask);
	dID = XDamageCreate(
		xDisplay, wID, XDamageReportNonEmpty);
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

WINDOW::P2 WINDOW::GetLocalPosition(const QON& d){
	QON dir(d);

	//頭の向きをベクタにして窓の中心を基準に変換
	VQON tgt(0, 0, 1);
	tgt.ReverseRotate(center);
	tgt.ReverseRotate(dir);

	//窓の上の位置に直す
	const float s(distance / (scale * tgt.k));
	P2 r = { (float)(tgt.i * s) + width * 0.5f,
		(float)(-tgt.j * s) + height * 0.5f };
	return r;
}

void WINDOW::Move(int x, int y){
	horiz = ((x + width*0.5) - (float)rootWidth/2.0) * scale;
	vert = ((y + height*0.5) - (float)rootHeight/2.0) * scale;

	//四元数表現
	QON::ROTATION v = { vert, 1, 0, 0 };
	QON::ROTATION h = { horiz, 0, 1, 0 };
	QON hq(h);
	QON vq(v);
	hq *= vq;
	center = hq;

	//フレームバッファ上の窓も移動
	XMoveWindow(xDisplay, wID, x, y);
	vx = x;
	vy = y;
printf("%lu:%d %d(%d %d).\n", wID, x, y, width, height);
}


void WINDOW::Focus(){
	if(focused != this){
		XSetInputFocus(xDisplay, wID, RevertToParent, CurrentTime);
		focused = this;
	}
}

void WINDOW::UnFocus(){
	if(focused == this){
		focused = 0;
	}
}

