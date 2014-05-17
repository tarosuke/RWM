
#include <stdlib.h>

#include "form.h"


const FORM* FORM::forms(0); //形状情報の配列(読み込み完了後に有効)
const FORM::VERTEX* FORM::vertexes(0); //頂点データ
TOOLBOX::QUEUE<FORM> FORM::stocks;
TOOLBOX::QUEUE<FORM> FORM::drawTargets;


///// 描画関連

void FORM::DrawAll(){
	for(TOOLBOX::QUEUE<FORM>::ITOR i(drawTargets); i; i++){
		(*i).Draw();
	}
}

void FORM::DrawTransparents(){}




void FORM::Draw(){
}




///// パケット関連

FORM::FORM(const HEADER& h) :
	PACKET(h, malloc(h.length)),
	stockNode(*this),
	drawNode(*this){}



void FORM::Do(SOCKET& s){
}


