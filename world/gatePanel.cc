/******************************************************************* gatepanel
 * 他の部屋への穴
 */
#include "gatePanel.h"
#include "texset.h"
#include "room.h"

#include <GL/gl.h>
#include <math.h>
#include <stdio.h>



GATEPANEL::GATEPANEL(WORLD& world, ROOM& into, const PROFILE& profile_) :
	GATE(into),
	profile(profile_),
	to(world, profile_.toRoom),
	dx(profile_.p1.x - profile_.p0.x),
	dz(profile_.p1.z - profile_.p0.z),
	norm(sqrt(dx*dx + dz*dz)),
	dnx(dx / norm),
	dnz(dz / norm){};



void GATEPANEL::Draw(unsigned remain, const TEXSET& texSet){
	const ROOM* const next(to);
	glFrontFace(GL_CW);
	if(!next){
		//リンク先がない場合
		glColorMask(1,1,1,1);
		glDepthMask(1);
		texSet.Bind(4);
		glBegin(GL_TRIANGLE_STRIP);
		 glTexCoord2f(0.9 / 0.3, 2 / 0.3);
		 glVertex3f(profile.p0.x, profile.floorHeight, profile.p0.z);
		 glTexCoord2f(0.9 / 0.3, 0);
		 glVertex3f(profile.p0.x, profile.ceilHeight, profile.p0.z);
		 glTexCoord2f(0, 2 / 0.3);
		 glVertex3f(profile.p1.x, profile.floorHeight, profile.p1.z);
		 glTexCoord2f(0, 0);
		 glVertex3f(profile.p1.x, profile.ceilHeight, profile.p1.z);
		glEnd();
	}else{
		//リンク先がある場合
		glColorMask(0,0,0,0);
		glDepthMask(0);
		glBegin(GL_TRIANGLE_STRIP);
		 glVertex3f(profile.p0.x, profile.floorHeight, profile.p0.z);
		 glVertex3f(profile.p0.x, profile.ceilHeight, profile.p0.z);
		 glVertex3f(profile.p1.x, profile.floorHeight, profile.p1.z);
		 glVertex3f(profile.p1.x, profile.ceilHeight, profile.p1.z);
		glEnd();
		(*next).Draw(remain);

		//隣の部屋を描画して壊れた設定を元に戻す
		glStencilFunc(GL_ALWAYS, remain, ~0);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_CULL_FACE);
	}
}


void GATEPANEL::Collision(class OBJECT& o){
	//oの相対位置
	VQON op(-profile.p0.x, 0, -profile.p0.z);
	op += o.Position();
	VQON ov(o.Velocity());

	//壁ベクトルとoの相対位置の内積を計算(正なら内側)
	const float distance(op.k*dnx - op.i*dnz);
	if(distance < 0){
		ROOM* const next(to);
		if(next && profile.throuable){
			o.MoveTo(*next);
		}else{
			//TODO:反発
		}
	}
}

