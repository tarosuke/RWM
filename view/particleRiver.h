#pragma once


#include "particles.h"


class PARTICLESRIVER : public PARTICLES{
	PARTICLESRIVER();
	PARTICLESRIVER(const PARTICLESRIVER&);
	void operator=(const PARTICLESRIVER&);
public:
	PARTICLESRIVER(
		unsigned size,
		const class IMAGE* sprite,
		unsigned numOfParticles,
		float xMin, float xMax,
		float yMin, float yMax,
		float zMin, float zMax,
		float xv, float yv, float zv);
private:
	class P : public PARTICLE{
	public:
		P(PARTICLESRIVER& p);
	private:
		PARTICLESRIVER& particles;
		void Update();
		static float R(float min, float max);
	};
	struct RANGE{
		RANGE(float min, float max) : min(min), max(max){};
		float min;
		float max;
	}x, y, z;
	struct VELOCITY{
		VELOCITY(float x, float y, float z) : x(x), y(y), z(z){};
		float x;
		float y;
		float z;
	}velocity;
};

