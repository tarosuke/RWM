#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "avatar.h"
#include <room/room.h>
#include <ghost/ghost.h>
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
	const double rotAccelRatio(1.0 / (10.0 + fabsl(rotVelocity)));
	rotVelocity = rotVelocity * (1.0 - rotAccelRatio) +
			ghost.GetRotate() * rotAccelRatio;
	QON::ROTATION r = { -rotVelocity * dt, 0, 1, 0 };
	QON rot(r);
	direction *= ghost.PickHeadHorizDir(0.01);
	direction *= rot;
	rotVelocity *= 0.9;

	//位置更新
	const double accelRatio(1.0 / (10.0 + velocity.Length()));
	VQON accel(ghost.GetSideStep(), 0.0, -ghost.GetForwardStep());
	accel *= accelRatio;
	const double acc(accel.Length());
	if(0.001 < fabsl(acc)){
		accel.Rotate(direction);
		accel *= acc * dt;

		velocity += accel;
	}
	in = &((*in).Move)(position.i, position.k, velocity.i, velocity.k, 0.3);
	velocity *= 0.95;
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

