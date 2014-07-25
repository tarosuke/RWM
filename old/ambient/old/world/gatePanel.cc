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
	glStencilFunc(GL_EQUAL, remain, ~0);
	glStencilOp(GL_KEEP, GL_DECR, GL_DECR);
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
		(*next).Draw(remain - 1);

		//隣の部屋を描画して壊れた設定を元に戻す
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_CULL_FACE);
	}
}


bool GATEPANEL::Collision(class OBJECT& o){
	//oの半径
	VQON radius(o.Radius());

	//oの相対位置
	VQON op(-profile.p0.x, 0, -profile.p0.z);
	op += o.Position();
	VQON ov(o.Velocity());

	//壁ベクトルとoの相対位置の内積を計算(正なら内側)
	const float distance(op.k*dnx - op.i*dnz);
	op -= ov;
	const float d2(op.k*dnx - op.i*dnz);
	if(distance < 0.15 && 0.15 <= d2 && profile.throuable){
		ROOM* const next(to);
		o.MoveTo(*next);
		return true;
	}else if(profile.throuable && distance < radius.i){
		const float dot(op.i*dnx + op.k*dnz);
		return 0 <= dot && dot <= 1;
	}
	return false;
}

