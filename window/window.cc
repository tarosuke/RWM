#include "window.h"


WINDOW::Q WINDOW::windowList;
WINDOW::Q WINDOW::invisibleWindowList;
WINDOW* WINDOW::focused(0);


float WINDOW::motionDistance(0.7);
bool WINDOW::lookingFront(false);
WINDOW::POINT WINDOW::lookingPoint;
WINDOW* WINDOW::lookingWindow(0);
const float WINDOW::baseDistance(0.7);
float WINDOW::distance;

void WINDOW::DrawAll(const COMPLEX<4>& pose){
	//仮想画面上の視線の先を算出
	VECTOR<3> viewLine((const double[]){ 0, 0, 1 });
	viewLine.Rotate(pose);
	const double* const v(viewLine);
	if(v[2] <= 0){
		//後ろ向いてるので一切描画しない
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
		const POINT lp = {
			lookingPoint.x - w.position.x,
			lookingPoint.y - w.position.y };
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

	//フォーカス窓描画
	if(focused){
		glColor4f(1,1,1,1); //白、不透明
		(*focused).Draw();
	}
}

void WINDOW::DrawTransparentAll(const COMPLEX<4>& pose){
	if(!lookingFront){
		//後ろ向いてるので一切描画しない
		return;
	}

	//非フォーカス
	glColor4f(1,1,1,0.7); //白、半透明
	for(WINDOW::Q::ITOR i(WINDOW::windowList, WINDOW::Q::ITOR::backward); i; --i){
		WINDOW* const w(i);
		if(w != focused){
			(*w).Draw();
		}
	}
}






void WINDOW::Draw(){
}

