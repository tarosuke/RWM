#include <GL/gl.h>
#include <GL/glx.h>

#include "view.h"
#include <rift/rift.h>
#include <toolbox/qon/glqon.h>


TOOLBOX::QUEUE<VIEW> VIEW::views;


VIEW::VIEW(int width_, int height_, AVATAR& avatar_) :
	node(*this),
	width(width_),
	height(height_),
	avatar(avatar_),
	displayList(glGenLists(1)){
	views.Add(node);

	//画面の実位置を仮定(スクリプトで上書き予定)
	realWidth = defaultDotPitch * width;
	realHeight = defaultDotPitch * height;
	realDistance = defaultDisplayDistance;
}

VIEW::~VIEW(){
	glDeleteLists(displayList, 1);
	delete &avatar;
}

void VIEW::DrawForEye() const{
	avatar.RoomIn().Draw(roomFollowDepth); //avatarがいる部屋から描画
}


void VIEW::Draw() const{
	//描画
	const float sr(nearDistance / (realDistance * 2));
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-realWidth * sr, realWidth * sr,
		-realHeight * sr, realHeight * sr,
		nearDistance, farDistance);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(0.5 , 1, 1);
	DrawForEye();
}


void VIEW::Update(float dt){
	avatar.RoomIn().Update(dt);
}


