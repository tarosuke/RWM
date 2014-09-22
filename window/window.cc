

#include <stdio.h>
#include "../gl/gl.h"

#include "window.h"


WINDOW::Q WINDOW::windowList;
WINDOW::Q WINDOW::invisibleWindowList;
WINDOW* WINDOW::focused(0);


float WINDOW::motionDistance(0.7);
WINDOW::POINT WINDOW::lookingPoint = {0, 0};
bool WINDOW::lookingFront(false);
WINDOW* WINDOW::lookingWindow(0);
const float WINDOW::baseDistance(0.7);
float WINDOW::distance;


void WINDOW::UpdateAll(const COMPLEX<4>& pose){
	//仮想画面上の視線の先を算出
	VECTOR<3> viewLine((const double[]){ 0, 0, 1 });
	viewLine.Rotate(pose);
	const double* const v(viewLine);
	if(v[2] <= 0){
		//後ろ向いてるので処理なし
		lookingFront = false;
		return;
	}
	lookingFront = true;
	const float vs(1.0 / (v[2] * scale));
	const POINT newLookingPont = {
		(float)-v[0] * motionDistance * vs,
		(float)-v[1] * motionDistance * vs };
	bool moved(false);
	if((int)lookingPoint.x != (int)newLookingPont.x
		|| (int)lookingPoint.y != (int)newLookingPont.y){
		moved = true;
		lookingPoint = newLookingPont;
	}
	//窓視点チェック
	WINDOW* const oldLookingWindow(lookingWindow);
	lookingWindow = 0;
	POINT localPoint;
	for(WINDOW::Q::ITOR i(windowList); i; i++){
		WINDOW& w(*i);
		const POINT lp(w.GetLocalPoint(lookingPoint));
		if(0 <= lp.x && lp.x < w.width && 0 <= lp.y && lp.y < w.height){
			//lookingPointが窓に含まれる
			lookingWindow = i;
			localPoint = lp;
			break;
		}else{
			moved = false;
		}
	}
	//Enter/Leaveイベント生成
	if(oldLookingWindow != lookingWindow){
		if(oldLookingWindow){
			//Leave
			MOUSE_LEAVE_EVENT e;
			(*oldLookingWindow).AtMouse(e);
		}
		if(lookingWindow){
			//Enter
			MOUSE_ENTER_EVENT e;
			(*lookingWindow).AtMouse(e);
		}
	}else if(moved && lookingWindow){
		//movedイベント生成
		MOUSE_MOVE_EVENT e;
		(*lookingWindow).AtMouse(e);
	}

	//ディスプレイリストとかやるのは後
//	windowList.Each(&WINDOW::Update);
}

void WINDOW::DrawAll(){
	if(!lookingFront){
		//後ろ向きなので描画しない
		return;
	}

	//フォーカス窓描画
	focused = windowList.Peek();
	if(focused){
		//視線の先を中心に
		glPushMatrix();
		glTranslatef(-lookingPoint.x * scale, -lookingPoint.y * scale, 0);

		glColor4f(1,1,1,1); //白、不透明
		(*focused).Draw(baseDistance);

		glPopMatrix();
	}
}

void WINDOW::DrawTransparentAll(){
	if(!lookingFront){
		//後ろ向いてるので一切描画しない
		return;
	}

	//視線の先を中心に
	glPushMatrix();
	glTranslatef(-lookingPoint.x * scale, -lookingPoint.y * scale, 0);

	//非フォーカス
	glColor4f(1,1,1,0.7); //白、半透明
	float d(baseDistance);
	for(WINDOW::Q::ITOR i(WINDOW::windowList, WINDOW::Q::ITOR::backward); i; --i){
		d += 0.03;
		WINDOW* const w(i);
		if(w != focused){
			(*w).Draw(d);
		}
	}

	glPopMatrix();
}




const float WINDOW::scale(0.0011);

void WINDOW::Draw(float distance){
	//描画位置算出
	const float h(position.x * scale);
	const float v(position.y * scale);
	if(M_PI*0.5 <= h*h + v*v){
		//エイリアスやどのみち見えない領域は描画しない
		//TODO:計算が大雑把で大嘘なのでただす
		return;
	}
	glColor4f(1,1,1,1); //白、不透明

	//テクスチャ割り当て
	GL::TEXTURE::BINDER binder(texture);

	//移動
	glPushMatrix();
	glTranslatef(h, -v, 0);

	//描画
	const float w2(width * scale * 0.5);
	const float h2(height * scale * 0.5);
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

	glPopMatrix();
}


WINDOW::POINT WINDOW::GetLocalPoint(const WINDOW::POINT& p){
	return (POINT){ p.x - position.x + width/2, position.y - p.y - height/2 };
}


////////////////////////////////////////////////////////イベントの一次ハンドラ
void WINDOW::AtMouse(MOUSE_EVENT& e){
	if(!lookingWindow){
		return;
	}
	WINDOW& w(*lookingWindow);
	const POINT p(w.GetLocalPoint(lookingPoint));
	e.x = p.x;
	e.y = p.y;
	e.Handle(w);
}

void WINDOW::AtKey(KEY_EVENT& e){
	if(!focused){
		//イベントを送る先がない
		return;
	}

	//イベント転送
	e.Handle(*focused);
}

void WINDOW::AtJS(JS_EVENT& e){
}





void WINDOW::Assign(const IMAGE& o){
	texture.Assign(o);
}


void WINDOW::Update(const IMAGE& o, int x, int y){
	texture.Update(o, x, y);
}


void WINDOW::Move(float x, float y){
	position = (POINT){ x, y };
}

void WINDOW::Resize(unsigned w, unsigned h){
	width = w;
	height = h;
}


WINDOW::WINDOW(float h, float v, int wi, int hi) :
	node(*this),
	visible(false),
	position((POINT){ h, v }),
	width(wi),
	height(hi){
	SetVisibility(false);
}

WINDOW::WINDOW(float h, float v, const IMAGE& image) :
	node(*this),
	texture(image),
	visible(true),
	position((POINT){ h, v }),
	width(image.Width()),
	height(image.Height()){
	SetVisibility(true);
}

WINDOW::~WINDOW(){
	if(this == lookingWindow){
		lookingWindow = 0;
	}
}


void WINDOW::SetVisibility(bool v){
	if(v != visible){
		node.Attach(v ? windowList : invisibleWindowList);
		visible = v;
	}
}



