#include "window.h"


TOOLBOX::QUEUE<WINDOW> WINDOW::windowList;
TOOLBOX::QUEUE<WINDOW> WINDOW::invisibleWindowList;
WINDOW* WINDOW::focused(0);


float WINDOW::motionDistance(0.7);
bool WINDOW::lookingFront(false);
WINDOW::POINT WINDOW::lookingPoint;

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







	//フォーカス窓
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
	for(TOOLBOX::QUEUE<WINDOW>::ITOR i(WINDOW::windowList, true); i; --i){
		WINDOW* const w(i);
		if(w != focused){
			(*w).Draw();
		}
	}
}






void WINDOW::Draw(){
}

