
#include "../gl/gl.h"
#include "particleRiver.h"

#include <stdlib.h>


#include <stdio.h>

PARTICLESRIVER::PARTICLESRIVER(
	unsigned size,
	const class IMAGE* sprite,
	unsigned numOfParticles,
	float xMin, float xMax,
	float yMin, float yMax,
	float zMin, float zMax,
	float xv, float yv, float zv) :
	PARTICLES(size, sprite),
	x(xMin, xMax),
	y(yMin, yMax),
	z(zMin, zMax),
	velocity(xv, yv, zv){

	for(unsigned n(0); n < numOfParticles; ++n){
		new P(*this);
	}
}


PARTICLESRIVER::P::P(PARTICLESRIVER& p) :
	PARTICLE(p,
		 R(p.x.min, p.x.max),
		 R(p.y.min, p.y.max),
		 R(p.z.min, p.z.max)),
	particles(p){}

float PARTICLESRIVER::P::R(float min, float max){
	return PARTICLES::R() * (max - min) + min;
}


void PARTICLESRIVER::P::Update(){
	//移動
	x += particles.velocity.x;
	y += particles.velocity.y;
	z += particles.velocity.z;

	//境界条件
	if(x < particles.x.min) x = particles.x.max;
	if(y < particles.y.min) y = particles.y.max;
	if(z < particles.z.min) z = particles.z.max;
	if(particles.x.max < x) x = particles.x.min;
	if(particles.y.max < y) y = particles.y.min;
	if(particles.z.max < z) z = particles.z.min;
}

