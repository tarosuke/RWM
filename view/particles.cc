
#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>

#include "particles.h"


PARTICLES::PARTICLES(){
	for(unsigned n(0); n < numOfParticles; ++n){
		new PARTICLE(particles);
	}
	VIEW::RegisterExternals(*this);
}

PARTICLES::~PARTICLES(){
	//TODO:PARTICLEをdelete
}

void PARTICLES::Update(){
	GL::DisplayList::Recorder rec(drawList);
	particles.Each(&PARTICLE::Update);
}

void PARTICLES::Draw()const{
	glDisable(GL_LIGHTING);
	glPushMatrix();
	drawList.Playback();
	glPopMatrix();
}

float PARTICLES::R(){
	return (float)rand() / RAND_MAX;
};



PARTICLES::PARTICLE::PARTICLE(TOOLBOX::QUEUE<PARTICLE>& q) : node(*this){
	x = R() * 10 - 5;
	y = R() * 10;
	z = R() * 10 - 10;
	q.Add(node);
}

void PARTICLES::PARTICLE::Update(){
	x += R() * 0.005;
	y += R() * 0.005;
	z += R() * 0.005;

	const float r(3 / sqrt(x*x+y*y+z*z));

	if(1.0 <= r){
		glPointSize(r);
		glColor3f(1, 1, 1);
	}else{
		glPointSize(1);
		glColor4f(1, 1, 1, r);
	}
	glBegin(GL_POINTS);
	glVertex3f(x, y, z);
	glEnd();

	y -= 0.025;
	if(y < -1.6 ){
		y += 10.0;
	}
	if(x < -2.5) x += 5;
	if(2.5 < x) x -= 5;
	if(z < -2.5) z += 5;
	if(2.5 < z) z -= 5;
};
