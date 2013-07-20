/**************************************************** humanoid avatar:humanoid
 *
 */
#include <GL/gl.h>
#include <stdio.h>
#include <math.h>

#include <rift/rift.h>
#include "humanoid.h"
#include <toolbox/qon/glqon.h>
#include <ghost/ghost.h>


HUMANOID::HUMANOID(
	const class ROOM& initialRoom,
	const PROFILE& profile_,
	class GHOST& ghost_) :
	AVATAR(initialRoom, ghost_, profile_.SightHeight()),
	profile(profile_),
	sitting(false){
}

HUMANOID::~HUMANOID(){}


void HUMANOID::GetView() const{
	glTranslatef(0, -profile.eyeHeight, profile.eyeFrontOffset);
	AVATAR::GetView();
}

void HUMANOID::Update(float dt){
	//ボタン3が押されていたら立ったり座ったり
// 	if(8 & ghost.GetActions()){
// 		position.j += sitting ? 0.5 : -0.5;
// 		sitting = !sitting;
// 	}

	//座ってたら移動しない
	if(sitting){
		VQON v;
		velocity = v;
		rotVelocity = 0.0;
		return;
	}

	AVATAR::Update(dt);
}

void HUMANOID::Draw() const{
	//体
	glLineWidth(5);
	glPushMatrix();
	GetModel();
	glTranslatef(0, -profile.eyeHeight, -0.5);
	//torso
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -profile.torso.length, 0);
	glEnd();
	//arms
	ArmVertexes(GL_LINE_STRIP);
	glPointSize(10);
	ArmVertexes(GL_POINTS);
	//legs
	glTranslatef(0, -profile.torso.length, 0);
	LegVertexes(GL_LINE_STRIP);
	LegVertexes(GL_POINTS);
	glPopMatrix();
	//頭
	glPushMatrix();
	GetModel();
	glTranslatef(0, -profile.eyeHeight, -0.5);
	const GLQON headDir(ghost.GetHead());
	headDir.GetModel();
	glBegin(GL_LINE_LOOP);
	glVertex3f(-profile.defaultHeadRatio*0.3, 0, 0);
	glVertex3f(-profile.defaultHeadRatio*0.3, profile.head.height, 0);
	glVertex3f(profile.defaultHeadRatio*0.3, profile.head.height, 0);
	glVertex3f(profile.defaultHeadRatio*0.3, 0, 0);
	glEnd();
	glBegin(GL_POINTS);
	glVertex3f(-profile.eyeSideOffset, profile.eyeHeight, 0);
	glVertex3f(profile.eyeSideOffset, profile.eyeHeight, 0);
	glEnd();
	glPopMatrix();
}
void HUMANOID::ArmVertexes(int type) const{
	glBegin(type);
	glVertex3f(-profile.armOffset,
		-profile.arm.upperLength - profile.arm.lowerLength, 0);
	glVertex3f(-profile.armOffset, -profile.arm.upperLength, 0);
	glVertex3f(-profile.armOffset, -profile.head.neck, 0);
	glVertex3f(profile.armOffset, -profile.head.neck, 0);
	glVertex3f(profile.armOffset, -profile.arm.upperLength, 0);
	glVertex3f(profile.armOffset,
		-profile.arm.upperLength - profile.arm.lowerLength, 0);
	glEnd();
}
void HUMANOID::LegVertexes(int type) const{
	glBegin(type);
	glVertex3f(-profile.legOffset,
		-profile.leg.upperLength - profile.leg.lowerLength, 0);
	glVertex3f(-profile.legOffset, -profile.leg.upperLength, 0);
	glVertex3f(-profile.legOffset, 0, 0);
	glVertex3f(profile.legOffset, 0, 0);
	glVertex3f(profile.legOffset, -profile.leg.upperLength, 0);
	glVertex3f(profile.legOffset,
		-profile.leg.upperLength - profile.leg.lowerLength, 0);
	glEnd();
}



/*** 体のプロファイル
 * 身長とターミナルレシオで全体のパラメタを生成する
 * ターミナルレシオは末端の根本に対するサイズ比を決める
 * これが大きいほど大人的、男性的となる。等身もターミナルレシオから生成される
 */
HUMANOID::PROFILE::PROFILE(float tall, float tr, float wr){
	//まず比率のみでプロファイルを生成
	torso.length = 1.0;
	legOffset = defaultWaistRatio * wr;  //trでlegとarmの比率補正
	armOffset = defaultSholderRatio * wr;
	head.height = defaultHeadRatio;
	head.neck = defaultNeckRatio * tr;
	eyeHeight = (head.height - 0.15) * tr;
	arm.upperLength = defaultArmRatio * tr;
	arm.lowerLength = arm.upperLength * tr;
	leg.upperLength = defaultLegRatio * tr;
	leg.lowerLength = leg.upperLength * tr;

	//身長に合わせて補正
	const float tallRatio(tall / Tall());

	for(float* raw(&torso.length); raw <= &leg.lowerLength; raw++){
		*raw *= tallRatio;;
	}
}



/*** 姿勢
 *
 */
HUMANOID::POSTURE::POSTURE() : arms(M_PI / 4), legs(0){
}

HUMANOID::POSTURE::LIMBS::LIMBS(double initialDir) :
	left(initialDir), right(initialDir){
}

HUMANOID::POSTURE::LIMBS::LIMB::LIMB(double initialDir){
	sh.angle = -initialDir;
	sh.x = sh.z = 0;
	sh.y = 1;
}

