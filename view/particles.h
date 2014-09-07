
#pragma once

#include "view.h"
#include "../toolbox/queue/queue.h"
#include "../gl/displayList.h"


class PARTICLES : VIEW::DRAWER{
public:
	PARTICLES();
	~PARTICLES();
	void Update();
	void Draw() const;
private:
	static const unsigned numOfParticles = 1000;
	class PARTICLE{
	public:
		PARTICLE(TOOLBOX::QUEUE<PARTICLE>&);
		void Update();
	private:
		TOOLBOX::NODE<PARTICLE> node;
		float x;
		float y;
		float z;
	};

	TOOLBOX::QUEUE<PARTICLE> particles;
	GL::DisplayList drawList;

	static float R();
};

