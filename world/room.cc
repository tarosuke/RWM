/************************************************************* each space:ROOM
 *
 */
#include "room.h"
#include "world.h"
#include "object.h"

#include <avatar/avatar.h>

#include <GL/gl.h>
#include <stdio.h>


ROOM::ROOM(WORLD& into) :
	node(*this),
	world(into),
	texSet(into.GetTextureSet()){
	into.Add(node);
};

ROOM::~ROOM(){
	//TODO:所属オブジェクトを全てdelete
}




void ROOM::Draw(unsigned remain) const{
	if(!remain){
		//もう追いかけない
		return;
	}

	glEnable(GL_TEXTURE);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//部屋の明るさ
	const float brightness[4] = { 0.5, 0.5, 0.5, 1 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, brightness);

	//リンクオブジェクトを描画
	glStencilFunc(GL_ALWAYS, remain - 1, ~0);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	for(TOOLBOX::QUEUE<GATE>::ITOR i(gates); i; i++){
		(*i).Draw(remain - 1, texSet);
	}

	//壁や床、天井を描画
	glColorMask(1,1,1,1);
	glDepthMask(1);
	glStencilFunc(GL_EQUAL, remain, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	for(TOOLBOX::QUEUE<PANEL>::ITOR i(panels); i; i++){
		(*i).Draw(texSet);
	}

	//その他のオブジェクトを描画
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_CULL_FACE);
	for(TOOLBOX::QUEUE<OBJECT>::ITOR i(objects); i; i++){
		(*i).Draw(texSet);
	}
}

void ROOM::Update(float dt){
	for(TOOLBOX::QUEUE<OBJECT>::ITOR i(objects); i; i++){
		(*i).Update(dt);
	}

	//衝突処理
	for(TOOLBOX::QUEUE<OBJECT>::ITOR o(objects); o; o++){
		for(TOOLBOX::QUEUE<PANEL>::ITOR p(panels); p; p++){
			(*p).Collision(*o);
		}
	}
	for(TOOLBOX::QUEUE<OBJECT>::ITOR o(objects); o; o++){
		for(TOOLBOX::QUEUE<OBJECT>::ITOR p(objects); p; p++){
			OBJECT* const o0(o);
			OBJECT* const o1(p);
			if(o0 != o1){
				(*o1).Collision(*o0);
			}
		}
	}

	//TODO:通過処理
}

