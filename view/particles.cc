
#include <stdlib.h>
#include <math.h>

#include "../gl/gl.h"

#include "particles.h"
#include "../image/image.h"



const float PARTICLES::distanceAttenuation[] = { 0, 1, 0 };
GL::TEXTURE::PARAMS spriteParams = {
	wrap_s : GL_REPEAT,
	wrap_t : GL_REPEAT,
	filter_mag : GL_LINEAR,
	filter_min : GL_LINEAR,
	texture_mode : GL_REPLACE,
	pointSprite : true,
};


PARTICLES::PARTICLES(float size, const IMAGE* pImage) : sprite(0), size(size){
	//ポイントスプライト用テクスチャ確保
	if(pImage){
		glEnable(GL_TEXTURE_2D);
		sprite = new GL::TEXTURE(*pImage, spriteParams);
	}

	//パーティクル生成
	for(unsigned n(0); n < numOfParticles; ++n){
		new PARTICLE(*this);
	}
	VIEW::RegisterExternals(*this);
}

PARTICLES::~PARTICLES(){
	//TODO:PARTICLEをdelete

	if(sprite){
		delete sprite;
	}
}

void PARTICLES::Update(){
	GL::DisplayList::Recorder rec(drawList);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, distanceAttenuation);
	glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, minSize);
	glPointSize(size);
glPushMatrix();
glRotatef(90, -1, 0, 0);

	if(sprite){
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_POINT_SPRITE);
		GL::TEXTURE::BINDER b(*sprite);
		particles.Each(&PARTICLE::Update);
		glDisable(GL_POINT_SPRITE);
	}else{
		particles.Each(&PARTICLE::Update);
	}

glPopMatrix();
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

void PARTICLES::DrawTransparent()const{
	glDisable(GL_LIGHTING);
//	glPushMatrix();
	drawList.Playback();
//	glPopMatrix();
}

float PARTICLES::R(){
	return (float)rand() / RAND_MAX;
};



PARTICLES::PARTICLE::PARTICLE(PARTICLES& p) :
	node(*this),particles(p){
	x = R() * 1000 - 500;
	y = R() * 500;
	z = R() * 2 - 5;
	p.particles.Add(node);
}

void PARTICLES::PARTICLE::Update(){
	x += R() * 0.005;
	y += R() * 0.005;
	z += R() * 0.005;

	const float r(particles.size / sqrt(x*x+y*y+z*z));
	if(minSize <= r){
		glColor3f(1, 1, 1);
	}else{
		glColor4f(1, 1, 1, minSize / r);
	}
	glBegin(GL_POINTS);
	glVertex3f(x, y, z);
	glEnd();

	y -= 0.125;
	if(y < -1.6 ){
		y += 500.0;
	}
	if(x < -500) x += 1000;
	if(500 < x) x -= 1000;
// 	if(z < -2.5) z += 5;
// 	if(2.5 < z) z -= 5;
};
