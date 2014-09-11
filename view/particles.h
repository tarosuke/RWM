
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
protected:
	class PARTICLE{
		PARTICLE();
		PARTICLE(const PARTICLE&);
		void operator=(const PARTICLE&);
	public:
		PARTICLE(PARTICLES&,float,float,float);
		virtual ~PARTICLE(){};
		virtual void Update()=0;
		void Draw();
	protected:
		float x;
		float y;
		float z;
	private:
		TOOLBOX::NODE<PARTICLE> node;
		PARTICLES& particles;
	};

	static float R();
private:
	TOOLBOX::QUEUE<PARTICLE> particles;
	GL::DisplayList drawList;
	GL::TEXTURE* sprite;

	float size;
	static const float minSize = 1;

	static const float distanceAttenuation[];
};

