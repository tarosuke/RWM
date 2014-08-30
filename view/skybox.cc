///// 天箱

#include "../gl/gl.h"
#include "skybox.h"
#include "../image/image.h"
#include "../image/tga.h"


SKYBOX::SKYBOX(const char* path){
	TGAFile tga(path); //TODO:IMAGE側にファクトリっぽいものが必要
	texture.Assign(tga);

	Record();
	VIEW::RegisterExternals(*this);
}

SKYBOX::SKYBOX(const IMAGE& image) : texture(image){
	Record();
	VIEW::RegisterExternals(*this);
}


void SKYBOX::Record(){
//	DisplayList::Recorder rec(displayList);
}

void SKYBOX::Draw() const{
//	displayList.Playback();
	GL::TEXTURE::BINDER((*this).texture);
	const float p(500);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(2.0/4, 1.0/3);
	glVertex3f(p, p, -p);
	glTexCoord2f(2.0/4, 0.0/3);
	glVertex3f(p, p, p);
	glTexCoord2f(1.0/4, 1.0/3);
	glVertex3f(-p, p, -p);
	glTexCoord2f(1.0/4, 0.0/3);
	glVertex3f(-p, p, p);
	glVertex3f(-p, p, p);

	glTexCoord2f(0.0/4, 1.0/3);
	glVertex3f(-p, p, p);
	glVertex3f(-p, p, p);
	glTexCoord2f(0.0/4, 2.0/3);
	glVertex3f(-p, -p, p);
	glTexCoord2f(1.0/4, 1.0/3);
	glVertex3f(-p, p, -p);
	glTexCoord2f(1.0/4, 2.0/3);
	glVertex3f(-p, -p, -p);
	glTexCoord2f(2.0/4, 1.0/3);
	glVertex3f(p, p, -p);
	glTexCoord2f(2.0/4, 2.0/3);
	glVertex3f(p, -p, -p);
	glTexCoord2f(3.0/4, 1.0/3);
	glVertex3f(p, p, p);
	glTexCoord2f(3.0/4, 2.0/3);
	glVertex3f(p, -p, p);
	glTexCoord2f(4.0/4, 1.0/3);
	glVertex3f(-p, p, p);
	glTexCoord2f(4.0/4, 2.0/3);
	glVertex3f(-p, -p, p);
	glVertex3f(-p, -p, p);


	glTexCoord2f(2.0/4, 3.0/3);
	glVertex3f(p, -p, p);
	glVertex3f(p, -p, p);
	glTexCoord2f(2.0/4, 2.0/3);
	glVertex3f(p, -p, -p);
	glTexCoord2f(1.0/4, 3.0/3);
	glVertex3f(-p, -p, p);
	glTexCoord2f(1.0/4, 2.0/3);
	glVertex3f(-p, -p, -p);

	glEnd();
}

