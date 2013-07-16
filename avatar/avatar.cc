#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "avatar.h"
#include <room/room.h>
#include <ghost.h>
#include <toolbox/qon/glqon.h>


AVATAR::AVATAR(const ROOM& initialRoom, GHOST& g, float y) :
	in(&initialRoom), roomNode(*this),
	ghost(g){
	direction.Normalize();
	float x, h, z;
	initialRoom.GetCenter(x, h, z);
	VQON p(x, h + y, z);
	position = p;
	(*in).RegisterAvatar(roomNode);;
}

AVATAR::~AVATAR(){}



void AVATAR::Update(float dt){
	//向き更新
	direction *= ghost.PickHeadHorizDir(0.01);
	direction *= rotVelocity;
	rotVelocity *= 0.8;

	//位置更新
	in = &((*in).Move)(position.i, position.k, velocity.i, velocity.k, 0.3);
	velocity *= 0.9;
}

void AVATAR::GetView() const{
	//頭の向き設定
	const GLQON headDir(ghost.GetHead());
	headDir.GetView();

	//向き設定
	QON::ROTATION r;
	direction.GetRotation(r);
	glRotated(-r.angle * 180 / M_PI, r.x, r.y, r.z);

	//位置設定
	glTranslatef(-position.i, -position.j, -position.k);
}
void AVATAR::GetModel() const{
	//位置設定
	glTranslatef(position.i, position.j, position.k);

	//向き設定
	QON::ROTATION r;
	direction.GetRotation(r);
	glRotated(r.angle * 180 / M_PI, r.x, r.y, r.z);
}

void AVATAR::Draw(const int remain,
	const class TEXTURE& textures) const{
	assert(in);
	(*in).Draw(remain, textures);
}



void AVATAR::Rotate(float rotation){
	QON::ROTATION r = { -rotation, 0.0, 1, 0.0 };
	QON q(r);
	rotVelocity = q;
}

void AVATAR::Step(float forward, float right){
	VQON delta(right, 0.0, -forward);
	const double length(delta.Length());
	delta.Rotate(direction);
	delta *= length;
	velocity = delta;
}
void AVATAR::ForwardStep(float forward){
	VQON delta(0.0, 0.0, -forward);
	const double length(delta.Length());
	delta.Rotate(direction);
	delta *= length;
	velocity = delta;
}
void AVATAR::SideStep(float right){
	VQON delta(right, 0.0, 0.0);
	const double length(delta.Length());
	delta.Rotate(direction);
	delta *= length;
	velocity = delta;
}
void AVATAR::Actions(unsigned){};

