
#pragma once

#include "view.h"
#include "../toolbox/queue/queue.h"
#include "../gl/displayList.h"
#include "../gl/texture.h"


class PARTICLES : VIEW::DRAWER{
public:
	PARTICLES(float size, const class IMAGE* particleImage = 0);
	~PARTICLES();
	void Update();
	void DrawTransparent() const;
private:
	static const unsigned numOfParticles = 1000;
	class PARTICLE{
	public:
		PARTICLE(PARTICLES&);
		void Update();
	private:
		TOOLBOX::NODE<PARTICLE> node;
		float x;
		float y;
		float z;
		PARTICLES& particles;
	};

	TOOLBOX::QUEUE<PARTICLE> particles;
	GL::DisplayList drawList;
	GL::TEXTURE* sprite;

	float size;
	static const float minSize = 3;

	static const float distanceAttenuation[];

	static float R();
};

