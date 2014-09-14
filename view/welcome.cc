
#include <stdio.h>
#include <math.h>

#include "../gl/gl.h"
#include "welcome.h"
#include "../image/png.h"
#include "../image/tga.h"


WELCOME::WELCOME(VIEW& view, const char* pngFile) :
	texture(PNG(pngFile)), finished(false), vanish(false), view(view){
	VIEW::RegisterStickies(*this);
}


void WELCOME::Update(){
	angle = duration * 90 - 90;

	duration += frameDuration;
	if(1.0 <= view.InitialProgress()){
		finished = true;
	}

	//消滅処理
	const float co(cosf(angle * M_PI / 180));
	if(finished && 0 <= co){
		vanish = true;
	}
	if(vanish && co < 0){
		delete this;
	}
}

void WELCOME::Draw()const{
	glPushMatrix();

	glTranslatef(0,-0.075,-0.5);
	glRotatef(angle, 0, 1, 0);

	GL::TEXTURE::BINDER b(texture);

	if(duration < 1.2 || vanish){
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_FRONT);
	}

	glColor3f(1,1,1);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0, 0);
	glVertex3f(-width, height, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-width, -height, 0);
	glTexCoord2f(1, 0);
	glVertex3f(width, height, 0);
	glTexCoord2f(1, 1);
	glVertex3f(width, -height, 0);
	glEnd();

	glDisable(GL_CULL_FACE);

	glPopMatrix();
}

