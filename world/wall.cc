/************************************************************* 部屋の壁とか：WALL
 *
 */
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>

#include "room.h"
#include "wall.h"
#include "texset.h"


WALL::WALL(ROOM& room, const PROFILE& profile_) :
	PANEL(room),
	profile(profile_),
	dx(profile_.p1.x - profile_.p0.x),
	dz(profile_.p1.y - profile_.p0.y),
	norm(sqrt(dx*dx + dz*dz)),
	dnx(dx / norm), dnz(dz / norm),
	txu0(profile_.texOffset.x),
	txu1(txu0 + (norm / profile_.texSize.x)),
	txv0(profile_.texOffset.y),
	txv1(txv0 + (profile_.ceilHeight - profile_.floorHeight) /
		profile_.texSize.y){
};

WALL::~WALL(){}

void WALL::Draw(const TEXSET& texSet){
	texSet.Bind(profile.textureID);
	const float baseColor[4] = { 0.5, 0.5, 0.5, 1 };
	glMaterialfv(GL_FRONT_AND_BACK,
		GL_AMBIENT_AND_DIFFUSE, baseColor);
	glNormal3f(-dnz, 0, dnx);
	glFrontFace(GL_CW);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(txu0, txv0);
	glVertex3f(profile.p0.x, profile.floorHeight, profile.p0.y);
	glTexCoord2f(txu0, txv1);
	glVertex3f(profile.p0.x, profile.ceilHeight, profile.p0.y);
	glTexCoord2f(txu1, txv0);
	glVertex3f(profile.p1.x, profile.floorHeight, profile.p1.y);
	glTexCoord2f(txu1, txv1);
	glVertex3f(profile.p1.x, profile.ceilHeight, profile.p1.y);
	glEnd();
}

void WALL::Collision(OBJECT& o){
	//壁の法線
	const float nx(dnz);
	const float ny(-dnx);

	//oの相対位置(oの半径でオフセット)
	const VQON ora(o.Radius());
	VQON op(nx * ora.i - profile.p0.x, 0, ny * ora.k - profile.p0.y);
	op += o.Position();
	VQON ov(o.Velocity());

	//壁ベクトルとoの相対位置の内積を計算(正なら内側)
	const float distance(op.k*dnx - op.i*dnz);
	if(distance < 0){
		VQON a(nx * distance * 2, 0, ny * distance * 2);
		o.Accel(a);
	}
}
