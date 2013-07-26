/***************************************************** 床、天井などの水平面:PLANE
 *
 */
#include <GL/gl.h>
#include <stdio.h>

#include "room.h"
#include "plane.h"
#include "texset.h"


PLANE::PLANE(ROOM& room, const PROFILE& profile_) :
	PANEL(room),
	profile(profile_),
	normaly(profile_.floor ? 1 : -1),
	face(profile_.floor ? GL_CW : GL_CCW){}

PLANE::~PLANE(){}

void PLANE::Draw(const TEXSET& texSet){
	texSet.Bind(profile.textureID);
	const float baseColor[4] = { 0.5, 0.5, 0.5, 1 };
	glMaterialfv(GL_FRONT_AND_BACK,
		     GL_AMBIENT_AND_DIFFUSE, baseColor);
	glNormal3f(0, normaly, 0);
	glFrontFace(face);
	glBegin(GL_TRIANGLE_FAN);
	for(unsigned n(0); n < profile.numOfCorners; n++){
		const float x(profile.points[n].x);
		const float z(profile.points[n].y);
		glTexCoord2f(profile.texOffset.x + x / profile.texSize.x,
			profile.texOffset.y + z / profile.texSize.y);
		glVertex3f(x, profile.height, z);
	}
	glEnd();
}

void PLANE::Collision(OBJECT&){
}
