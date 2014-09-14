
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

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
	//初期職設定
	color.r = color.g = color.b = color.a = 1;

	//ポイントスプライト用テクスチャ確保
	if(pImage){
		glEnable(GL_TEXTURE_2D);
		sprite = new GL::TEXTURE(*pImage, spriteParams);
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

	if(sprite){
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_POINT_SPRITE);
		GL::TEXTURE::BINDER b(*sprite);
		particles.Each(&PARTICLE::Update);
		particles.Each(&PARTICLE::Draw);
		glDisable(GL_POINT_SPRITE);
	}else{
		particles.Each(&PARTICLE::Update);
		particles.Each(&PARTICLE::Draw);
	}

	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

void PARTICLES::DrawTransparent(){
	glDisable(GL_LIGHTING);
	drawList.Playback();
}

void PARTICLES::SetColor(float r, float g, float b, float a){
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
}

float PARTICLES::R(){
	return (float)rand() / RAND_MAX;
};



PARTICLES::PARTICLE::PARTICLE(PARTICLES& p, float x, float y, float z) :
	x(x), y(y), z(z), node(*this), particles(p){ p.particles.Add(node); }

void PARTICLES::PARTICLE::Draw(){
	const float r(particles.size / sqrt(x*x+y*y+z*z));
	if(minSize <= r){
		glColor4fv(particles.color.raw);
	}else{
		glColor4f(
			particles.color.r,
			particles.color.g,
			particles.color.b,
			particles.color.a * r / minSize);
	}
	glBegin(GL_POINTS);
	glVertex3f(x, y, z);
	glEnd();
};
