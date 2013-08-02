/******************************************************************* GATEPLANE
 * 他の部屋への穴(垂直)
 */
#include <GL/gl.h>
#include <stdio.h>

#include "room.h"
#include "gatePlane.h"
#include "texset.h"


GATEPLANE::GATEPLANE(WORLD& world, ROOM& into, const PROFILE& profile_) :
	GATE(into),
	profile(profile_),
	normaly(profile_.floor ? 1 : -1),
	face(profile_.floor ? GL_CW : GL_CCW),
	to(world, profile_.toRoom){}

GATEPLANE::~GATEPLANE(){}

void GATEPLANE::Draw(unsigned remain, const TEXSET& texSet){
	const ROOM* const next(to);
	glNormal3f(0, normaly, 0);
	glFrontFace(face);
	if(!next){
		//リンク先がない場合
		glColorMask(1,1,1,1);
		glDepthMask(1);
		texSet.Bind(4);
		glBegin(GL_TRIANGLE_FAN);
		for(unsigned n(0); n < profile.numOfCorners; n++){
			const float x(profile.points[n].x);
			const float z(profile.points[n].y);
			glTexCoord2f(x / 0.3, z / 0.3);
			glVertex3f(x, profile.height, z);
		}
		glEnd();
	}else{
		//リンク先がある場合
		glBegin(GL_TRIANGLE_FAN);
		for(unsigned n(0); n < profile.numOfCorners; n++){
			glColorMask(0,0,0,0);
			glDepthMask(0);
			const float x(profile.points[n].x);
			const float z(profile.points[n].y);
			glVertex3f(x, profile.height, z);
		}
		glEnd();

		//隣の部屋を描画
		(*next).Draw(remain);

		//隣の部屋を描画して壊れた設定を元に戻す
		glStencilFunc(GL_ALWAYS, remain, ~0);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_CULL_FACE);
	}
}

bool GATEPLANE::Collision(OBJECT&){
	return false;
}
