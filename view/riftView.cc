/***************************************************** view with rift:riftView
 *
 */
#include <GL/gl.h>
#include <GL/glx.h>
#include "riftView.h"
#include <world/room.h>
#include <rift/rift.h>
#include <avatar/avatar.h>
#include <toolbox/qon/glqon.h>


RIFTVIEW::RIFTVIEW(AVATAR& avatar) :
	VIEW(1280, 800, avatar){
}

RIFTVIEW::~RIFTVIEW(){}


void RIFTVIEW::Draw() const{
	const float sr(nearDistance / (realDistance * 2));
	const int hw(width / 2);
	const int rhw(realWidth / 2);
	const double inset(0.17);

	//左目
	glViewport(0, 0, hw, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum((-rhw - inset) * sr, (rhw - inset) * sr,
			-realHeight * sr, realHeight * sr,
	nearDistance, farDistance);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.03, 0, 0);
	avatar.GetView();
	static bool drawn(false);
	if(!drawn){
		glNewList(displayList, GL_COMPILE_AND_EXECUTE);
		DrawForEye();
		glEndList();
		// 			drawn = true;
	}else{
		glCallList(displayList);
	}

	//右目
	glViewport(hw, 0, hw, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum((-rhw + inset) * sr, (rhw + inset) * sr,
			-realHeight * sr, realHeight * sr,
	nearDistance, farDistance);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.03, 0, 0);
	avatar.GetView();
	glCallList(displayList);

	//TODO:ここで描画バッファをテクスチャにして歪み付きで描画
}


void RIFTVIEW::Update(float dt){
	avatar.Update(dt);
}


