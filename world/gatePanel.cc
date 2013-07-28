/******************************************************************* gatepanel
 * 他の部屋への穴
 */
#include "gatePanel.h"
#include "texset.h"
#include "room.h"

#include <GL/gl.h>
#include <stdio.h>


void GATEPANEL::Draw(unsigned remain, const TEXSET& texSet){
	const ROOM* const next(to);
	glFrontFace(GL_CCW);
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
	}
}


void GATEPANEL::Collision(class OBJECT&){}; //TODO:通過処理











#if 0
#include "linkPanel.h"
#include "texture.h"


void LINKPANEL::Draw(unsigned remain, const TEXTURE& texture){
	//ステンシルバッファにだけ描画させる
	const ROOM* const next(link);

	glFrontFace(GL_CCW);
	if(!next){
		//リンク先がない場合
		glColorMask(1,1,1,1);
		glDepthMask(1);
		texture.Bind(4);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.9 / 0.3, 2 / 0.3);
			glVertex3f(0.6, 0, 3);
			glTexCoord2f(0.9 / 0.3, 0);
			glVertex3f(0.6, 2, 3);
			glTexCoord2f(0, 2 / 0.3);
			glVertex3f(1.5, 0, 3);
			glTexCoord2f(0, 0);
			glVertex3f(1.5, 2, 3);
		glEnd();
	}else{
		//TODO:リンク先がある場合
		glColorMask(0,0,0,0);
		glDepthMask(0);
		glBegin(GL_TRIANGLE_STRIP);
			glVertex3f(0.6, 0, 3);
			glVertex3f(0.6, 2, 3);
			glVertex3f(1.5, 0, 3);
 			glVertex3f(1.5, 2, 3);
		glEnd();
		(*next).Draw(remain, texture);
	}
}

#endif
