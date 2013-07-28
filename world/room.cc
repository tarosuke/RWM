/************************************************************* each space:ROOM
 *
 */
#include "room.h"
#include "world.h"
#include "object.h"

#include <avatar/avatar.h>

#include <GL/gl.h>


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
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
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

	//AVATARを描画
	for(TOOLBOX::QUEUE<AVATAR>::ITOR i(avatars); i; i++){
		(*i).Draw(texSet);
	}
}

void ROOM::Update(float dt){
	for(TOOLBOX::QUEUE<OBJECT>::ITOR i(objects); i; i++){
		(*i).Update(dt);
	}
	for(TOOLBOX::QUEUE<AVATAR>::ITOR i(avatars); i; i++){
		(*i).Update(dt);
	}

	//TODO:衝突判定、再移動
// 	for(TOOLBOX::QUEUE<OBJECT>::ITOR i(movables); i; i++){
// 		CollisionObjects(i, links);
// 		CollisionObjects(i, inMovables);
// 		CollisionObjects(i, movables);
// 	}
}

