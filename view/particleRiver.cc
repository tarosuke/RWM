
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
	float zMin, float zMax) :
	PARTICLES(size, sprite),
	x(xMin, xMax),
	y(yMin, yMax),
	z(zMin, zMax){

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
	x *= 0.999;
	z *= 0.999;

#if 0
	x += R() * 0.005;
	y += R() * 0.005;
	z += R() * 0.005;

	y -= 0.125;
	if(y < -1.6 ){
		y += 500.0;
	}
	if(x < -500) x += 1000;
	if(500 < x) x -= 1000;
	// 	if(z < -2.5) z += 5;
	// 	if(2.5 < z) z -= 5;
#endif
}

