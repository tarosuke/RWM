#include <GL/glew.h>
#include <GL/gl.h>


#include <ambient.h>



void Ambient::Room::Draw(const unsigned level){
	//既に描画されていたら戻る
	if(sequence == Ambient::sequence){
		return;
	}
	sequence = Ambient::sequence;

	//ゲート描画
	glStencilFunc(GL_EQUAL, level, stencilBitMask);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glColorMask(0,0,0,0);
	glDepthMask(0);
	gates.Each(&Object::Draw);
	glColorMask(1,1,1,1);
	glDepthMask(1);

	//壁、床、天井描画
	glStencilFunc(GL_LESS, level, stencilBitMask); //その部屋のゲートには描画しない
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	borders.Each(&Object::Draw);

	//ゲート追跡
	if(level < maxLevel){
		for(TOOLBOX::QUEUE<Object>::ITOR i(gates); i; i++){
			Room* next((*i).next);
			if(next){
				(*next).Draw(level - 1);
			}
		}
	}

	//戻りで透過物体を描画(透過物体はゲートの上に描画するのでステンシルテストはなし)
	glDisable(GL_STENCIL_TEST);
	transparents.Each(&Object::Draw);
}

