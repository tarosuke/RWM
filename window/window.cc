#include "window.h"


TOOLBOX::QUEUE<WINDOW> WINDOW::queue;
WINDOW* WINDOW::focused(0);
WINDOW::DRAWER WINDOW::drawer;



void WINDOW::DRAWER::Draw() const{
	//フォーカス窓
	if(focused){
		glColor4f(1,1,1,1); //白、不透明
		(*focused).Draw();
	}

	//非フォーカス
	glColor4f(1,1,1,0.7); //白、半透明
	for(TOOLBOX::QUEUE<WINDOW>::ITOR i(WINDOW::queue, true); i; --i){
		WINDOW* const w(i);
		if(w != focused){
			(*w).Draw();
		}
	}
}


