
#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>

#include "snow.h"


SNOW::P SNOW::positions[numOfParticles];
SNOW SNOW::particles[numOfParticles];


void SNOW::DrawAll(){
	glDisable(GL_LIGHTING);

	for(unsigned n(0); n < numOfParticles; ++n){
		particles[n].Run();
	}
}

float SNOW::R(){
	return (float)rand() / RAND_MAX;
};



SNOW::SNOW() : p(positions[this - particles]){
	p.x = R() * 10 - 5;
	p.y = R() * 10;
	p.z = R() * 10 - 10;
}

void SNOW::Run(){
	p.x += R() * 0.005;
	p.y += R() * 0.005;
	p.z += R() * 0.005;

	const float r(3 / sqrt(p.x*p.x+p.y*p.y+p.z*p.z));

	if(1.0 <= r){
		glPointSize(r);
		glColor3f(1, 1, 1);
	}else{
		glPointSize(1);
		glColor4f(1, 1, 1, r);
	}
	glBegin(GL_POINTS);
	glVertex3f(p.x, p.y, p.z);
	glEnd();

	p.y -= 0.025;
	if(p.y < -1.6 ){
		p.y += 10.0;
	}
	if(p.x < -5) p.x += 10;
	if(5 < p.x) p.x -= 10;
	if(p.z < -10) p.z += 10;
	if(0 < p.z) p.z -= 10;
};
