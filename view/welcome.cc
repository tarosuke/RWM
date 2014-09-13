
#include <stdio.h>

#include "../gl/gl.h"
#include "welcome.h"
#include "../image/png.h"
#include "../image/tga.h"


WELCOME::WELCOME(const char* pngFile) : texture(PNG(pngFile)){
	VIEW::RegisterStickies(*this);
}


void WELCOME::Update(){
	duration += frameDuration;
}

void WELCOME::Draw()const{
	GL::TEXTURE::BINDER b(texture);
// 	glEnable(GL_CULL_FACE);

	glColor3f(1,1,1);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0, 0);
	glVertex3f(-width, 0, -0.5);
	glTexCoord2f(0, 1);
	glVertex3f(-width, -height*2, -0.5);
	glTexCoord2f(1, 0);
	glVertex3f(width, 0, -0.5);
	glTexCoord2f(1, 1);
	glVertex3f(width, -height*2, -0.5);
	glEnd();

// 	glDisable(GL_CULL_FACE);
}

