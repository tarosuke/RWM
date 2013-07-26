/************************************************************ 部屋の壁とか：panel
 *
 */
#if 0
#include <GL/gl.h>
#include <math.h>

#include "room.h"
#include "panel.h"
#include "texture.h"


PANEL(ROOM& room, const PROFILE& profile_) :
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
	room.RegisterWall(*this);
};

PANEL::~PANEL(){}

void PANEL::Draw(const TEXTURE& texture){
	texture.Bind(profile.textureID);
	glNormal3f(dnz, 0, -dnx);
	glBegin(GL_TRIANGLE_STRIP);
	glVartex3f(profile.p0.x, profile.floorHeight, profile.p0.z);
	glTexCoord2f(txu0, txv0);
	glVartex3f(profile.p0.x, profile.ceilHeight, profile.p0.z);
	glTexCoord2f(txu0, txv1);
	glVartex3f(profile.p1.x, profile.floorHeight, profile.p1.z);
	glTexCoord2f(txu1, txv0);
	glVartex3f(profile.p1.x, profile.ceilHeight, profile.p1.z);
	glTexCoord2f(txu1, txv1);
	glEnd();
}

#endif
