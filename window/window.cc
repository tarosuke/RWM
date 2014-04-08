/******************************************************* window handler:window
 */
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glx.h>

#include <toolbox/glpose/glpose.h>

#include <window.h>
#include <view/view.h>
#include <toolbox/cyclic/cyclic.h>



//窓までの距離
float WINDOW::baseDistance(0.5);
float WINDOW::motionDistance(2.0);

//窓全体制御関連
TOOLBOX::QUEUE<WINDOW> WINDOW::windowList;
WINDOW* WINDOW::focused(0);
const float WINDOW::scale(0.0011);


//窓生成
WINDOW::WINDOW() :
	node(*this),
	tID(0),
	visibility(false){
	//窓リストへ登録
	windowList.Insert(node);
}


WINDOW::WINDOW(float h, float v, int wi, int hi) :
	horiz(h),
	vert(v),
	width(wi),
	height(hi),
	node(*this),
	tID(0),
	visibility(false){
	//窓リストへ登録
	windowList.Insert(node);
}

WINDOW::WINDOW(float h, float v, const IMAGE& initialImage) :
	node(*this),
	tID(0),
	visibility(true){

	//テクスチャ割り当て
	AssignImage(initialImage);

	//窓リストへ登録
	windowList.Insert(node);

	//フォーカスを得る
	Focus();
}


// テクスチャ割り当て
void WINDOW::AssignImage(const IMAGE& image){
	AssignImage(image.GetMemoryImage(), image.GetWidth(), image.GetHeight());
}
void WINDOW::AssignImage(const void* bitmap, unsigned w, unsigned h){
	assert(bitmap);

	//テクスチャが既に割り当てられていたら入れ替えるために解放
	if(tID){
		glDeleteTextures(1, &tID);
	}

	//サイズ取得
	width = w;
	height = h;

	//テクスチャ割り当て
	glGenTextures(1, &tID);
	assert(tID);

	glBindTexture(GL_TEXTURE_2D, tID);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	const void* const memImage(bitmap);
	if(memImage){
		//テクスチャへ転送
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB,width, height, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, memImage);
	}else{
		//TODO:仮データを割り当てておく
		assert(false);
	}

	//テクスチャの属性を設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void WINDOW::UpdateImage(
	const IMAGE& image, //元イメージ
	unsigned dx, //書き込み先座標
	unsigned dy){
	UpdateImage(
		image.GetMemoryImage(),
		dx, dy,
		image.GetWidth(),
		image.GetHeight());
}

void WINDOW::UpdateImage(
	const void* bitmap, //元イメージ
	unsigned dx, //書き込み先座標
	unsigned dy,
	unsigned w, //転送サイズ
	unsigned h){
	if(!tID){
		//テクスチャなし
		return;
	}
	glBindTexture(GL_TEXTURE_2D, tID);
	glTexSubImage2D(
		GL_TEXTURE_2D, 0,
		dx,
		dy,
		w,
		h,
		GL_BGRA,
		GL_UNSIGNED_BYTE,
		bitmap);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void WINDOW::UpdateImage(
	const IMAGE& image, //元イメージ
	unsigned sx, //元イメージ上の座標
	unsigned sy,
	unsigned dx, //書き込み先座標
	unsigned dy,
	unsigned w, //転送サイズ
	unsigned h){
	if(!tID){
		//テクスチャなし
		return;
	}

	//サブイメージ取得
	const IMAGE subImage(image, sx, sx, w, h);

	//テクスチャアップデート
	UpdateImage(subImage, dx, dy);
}

WINDOW::~WINDOW(){
	UnFocus();
	if(tID){
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &tID);
	}
}

void WINDOW::Focus(){
	if(focused){
		(*focused).OnUnfocused();
	}
	if(this != focused){
		focused = this;
		OnFocused();
	}
}

void WINDOW::UnFocus(){
	if(this == focused){
		OnUnfocused();
		focused = 0;
	}
}

void WINDOW::Draw(float distance){
	//可視設定チェック
	if(!visibility){
		return;
	}

	//描画位置算出
	const float h(horiz * scale + lookingPoint.x);
	const float v(vert * scale - lookingPoint.y);
	if(M_PI*0.5 <= h*h + v*v){
		//エイリアスやどのみち見えない領域は描画しない
		return;
	}

	//最初に視野の中心がかかった窓を見てることにする
	const float w2(width * scale * 0.5);
	const float h2(height * scale * 0.5);
	if(!lookingWindow && (-w2 <= h && h < w2) && (-h2 <= v && v < h2)){
		lookingWindow = this;
		localLookingPoint.x = (-h + w2) / scale;
		localLookingPoint.y = (-v + h2) / scale;
	}

	//窓の向き＆表示位置計算
	glPushMatrix();
#if 0
	const float ha(-(h / distance) * 180 / M_PI);
	const float va(-(v / distance) * 180 / M_PI);
	const float d(sqrt(ha*ha + va*va));

	//向きと位置を設定
	glRotatef(d, va, ha, 0);
#else
	glTranslatef(h, -v, 0);
#endif
	//描画
	glBindTexture(GL_TEXTURE_2D, tID);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0, 0);
	glVertex3f(-w2, h2, -distance);
	glTexCoord2f(0, 1);
	glVertex3f(-w2, -h2, -distance);
	glTexCoord2f(1, 0);
	glVertex3f(w2, h2, -distance);
	glTexCoord2f(1, 1);
	glVertex3f(w2, -h2, -distance);
	glEnd();

	//後始末
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}

WINDOW* WINDOW::lookingWindow;
WINDOW::POINT WINDOW::lookingPoint;
void WINDOW::DrawAll(const COMPLEX<4>& pose){
	//描画中心点算出
	VECTOR<3> front((const double[]){ 0, 0, 1 });
	front.Rotate(pose);
	const double* const v(front);
	if(v[2] <= 0){
		//後ろ向き
		return;
	}
	lookingPoint.x = v[0] * motionDistance / v[2];
	lookingPoint.y = v[1] * motionDistance / v[2];

	//窓描画
	lookingWindow = 0;
	float dd(0.0);
	lookingWindow = 0;
	for(TOOLBOX::QUEUE<WINDOW>::ITOR i(windowList); i; i++, dd += 0.02){
		(*i).Draw(baseDistance + dd);
	}
}


//イベントの一次ハンドラ
void WINDOW::AtMouse(const MOUSE_EVENT& e){
	if(!lookingWindow){
		return;
	}
	WINDOW& w(*lookingWindow);
	MOUSE_EVENT ev(e);
	ev.x = w.localLookingPoint.x;
	ev.y = w.localLookingPoint.y;
	switch(e.type){
	case EVENT::mouseDown :
		w.OnMouseDown(ev);
		break;
	case EVENT::mouseUp :
		w.OnMouseUp(ev);
		break;
	default:
		break;
	}
}

void WINDOW::AtKey(const KEY_EVENT& e){
	if(!focused){
		//イベントを送る先がない
		return;
	}
	switch(e.type){
	case EVENT::keyDown :
	case EVENT::keyRepeated :
		(*focused).OnKeyDown(e);
		break;
	case EVENT::keyUp :
		(*focused).OnKeyUp(e);
		break;
	default:
		break;
	}
}

void WINDOW::AtJS(const JS_EVENT& e){
}




void WINDOW::Move(float h, float v){
	horiz = h;
	vert = v;
}

void WINDOW::Resize(unsigned w, unsigned h){
	width = w;
	height = h;
}

