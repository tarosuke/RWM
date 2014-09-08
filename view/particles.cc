
#include <stdlib.h>
#include <math.h>

#include "../gl/gl.h"

#include "particles.h"



const float PARTICLES::distanceAttenuation[] = { 0, 1, 0 };



PARTICLES::PARTICLES(float size) : size(size){
	for(unsigned n(0); n < numOfParticles; ++n){
		new PARTICLE(*this);
	}
	VIEW::RegisterExternals(*this);
}

PARTICLES::~PARTICLES(){
	//TODO:PARTICLEをdelete
}

void PARTICLES::Update(){
	GL::DisplayList::Recorder rec(drawList);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, distanceAttenuation);
	glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, minSize);
	glPointSize(size);
glPushMatrix();
glRotatef(90, -1, 0, 0);
	particles.Each(&PARTICLE::Update);
glPopMatrix();
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

void PARTICLES::DrawTransparent()const{
	glDisable(GL_LIGHTING);
	glPushMatrix();
	drawList.Playback();
	glPopMatrix();
}

float PARTICLES::R(){
	return (float)rand() / RAND_MAX;
};



PARTICLES::PARTICLE::PARTICLE(PARTICLES& p) :
	node(*this),particles(p){
	x = R() * 10 - 5;
	y = R() * 500;
	z = R() * 10 - 10;
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
	if(x < -2.5) x += 5;
	if(2.5 < x) x -= 5;
	if(z < -2.5) z += 5;
	if(2.5 < z) z -= 5;
};
