

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
	lookingPoint.x = v[0] * motionDistance / v[2];
	lookingPoint.y = v[1] * motionDistance / v[2];

	//窓視点チェック
	WINDOW* const oldLookingWindow(lookingWindow);
	lookingWindow = 0;
	POINT localPoint;
	for(WINDOW::Q::ITOR i(windowList); i; i++){
		WINDOW& w(*i);
		const POINT lp(lookingPoint - w.position);
		const float hw(w.width * 0.5);
		const float hh(w.height * 0.5);
		if(-hw <= lp.x && lp.x < hw && -hh <= lp.y && lp.y < hh){
			//lookingPointが窓に含まれる
			lookingWindow = i;
			localPoint = lp;
			break;
		}
	}

	//Enter/Leaveイベント生成
	if(oldLookingWindow != lookingWindow){
		if(oldLookingWindow){
			//Leave
			(*oldLookingWindow).OnLeave();
		}
		if(lookingWindow){
			//Enter
			(*lookingWindow).OnEnter(oldLookingWindow);
		}
	}

	//ディスプレイリストとかやるのは後
//	windowList.Each(&WINDOW::Update);
}

void WINDOW::DrawAll(){
	if(!lookingFront){
		//後ろ向きなので描画しない
		return;
	}

	//視線の先を中心に
	glPushMatrix();
	glTranslatef(lookingPoint.x, lookingPoint.y, 0);

	//フォーカス窓描画
	if(focused){
		glColor4f(1,1,1,1); //白、不透明
		(*focused).Draw(baseDistance);
	}

	glPopMatrix();
}

void WINDOW::DrawTransparentAll(){
	if(!lookingFront){
		//後ろ向いてるので一切描画しない
		return;
	}

	//視線の先を中心に
	glPushMatrix();
	glTranslatef(lookingPoint.x, lookingPoint.y, 0);

	//非フォーカス
	glColor4f(1,1,1,0.7); //白、半透明
	float d(baseDistance);
	for(WINDOW::Q::ITOR i(WINDOW::windowList, WINDOW::Q::ITOR::backward); i; --i){
		WINDOW* const w(i);
		if((*w).visible){
			if(w != focused){
				(*w).Draw(d);
			}
			d += 0.03;
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
		return;
	}

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
	return (POINT){ p.x - position.x, p.y - position.y};
}


////////////////////////////////////////////////////////イベントの一次ハンドラ
void WINDOW::AtMouse(MOUSE_EVENT& e){
	if(!lookingWindow){
		return;
	}
	WINDOW& w(*lookingWindow);
	w.position = w.GetLocalPoint(lookingPoint);
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
	node.Insert(invisibleWindowList);
}

WINDOW::WINDOW(float h, float v, const IMAGE& image) :
	node(*this),
	texture(image),
	visible(true),
	position((POINT){ h, v }),
	width(image.Width()),
	height(image.Height()){
	node.Insert(windowList);
}


void WINDOW::SetVisibility(bool v){
	visible = v;
	node.Insert(v ? windowList : invisibleWindowList);
}



