///// 天箱

#include <assert.h>
#include <string.h>

#include "../gl/gl.h"
#include "skybox.h"
#include "../image/image.h"
#include "../image/tga.h"


SKYBOX::SKYBOX(const char* path){
	Register(TGAFile(path));
}

SKYBOX::SKYBOX(const IMAGE& image){
	Register(image);
}

void SKYBOX::Register(const IMAGE& image){
	//テクスチャを割り当てる
	texture.Assign(image);

	//TODO:DisplayListに記録する

	//VIEWの描画対象にする(暫定)
	VIEW::RegisterExternals(*this);
}

void SKYBOX::Draw() const{
//	displayList.Playback();
	GL::TEXTURE::BINDER((*this).texture);
	static const float p(500);
	static const float q(1.0 / 4);
	static const float t(1.0 / 3);
	static const float h[5] = { 0, q, 2*q, 3*q, 4*q };
	static const float v[4] = { 0, t, 2*t, 3*t };

	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(h[2], v[1]);
	glVertex3f(p, p, -p);
	glTexCoord2f(h[2], v[0]);
	glVertex3f(p, p, p);
	glTexCoord2f(h[1], v[1]);
	glVertex3f(-p, p, -p);
	glTexCoord2f(h[1], v[0]);
	glVertex3f(-p, p, p);
	glVertex3f(-p, p, p);

	glTexCoord2f(h[0], v[1]);
	glVertex3f(-p, p, p);
	glVertex3f(-p, p, p);
	glTexCoord2f(h[0], v[2]);
	glVertex3f(-p, -p, p);
	glTexCoord2f(h[1], v[1]);
	glVertex3f(-p, p, -p);
	glTexCoord2f(h[1], v[2]);
	glVertex3f(-p, -p, -p);
	glTexCoord2f(h[2], v[1]);
	glVertex3f(p, p, -p);
	glTexCoord2f(h[2], v[2]);
	glVertex3f(p, -p, -p);
	glTexCoord2f(h[3], v[1]);
	glVertex3f(p, p, p);
	glTexCoord2f(h[3], v[2]);
	glVertex3f(p, -p, p);
	glTexCoord2f(h[4], v[1]);
	glVertex3f(-p, p, p);
	glTexCoord2f(h[4], v[2]);
	glVertex3f(-p, -p, p);
	glVertex3f(-p, -p, p);


	glTexCoord2f(h[2], v[3]);
	glVertex3f(p, -p, p);
	glVertex3f(p, -p, p);
	glTexCoord2f(h[2], v[2]);
	glVertex3f(p, -p, -p);
	glTexCoord2f(h[1], v[3]);
	glVertex3f(-p, -p, p);
	glTexCoord2f(h[1], v[2]);
	glVertex3f(-p, -p, -p);

	glEnd();
}

