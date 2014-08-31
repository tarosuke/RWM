///// 天箱

#include <assert.h>
#include <string.h>

#include "../gl/gl.h"
#include "skybox.h"
#include "../image/image.h"
#include "../image/tga.h"


const GL::TEXTURE::PARAMS SKYBOX::textureParams = {
	wrap_s : GL_CLAMP_TO_EDGE,
	wrap_t : GL_CLAMP_TO_EDGE,
	filter_mag : GL_NEAREST,
	filter_min : GL_NEAREST,
	texture_mode : GL_REPLACE,
};

SKYBOX::SKYBOX(const char* path){
	Register(TGAFile(path));
}

SKYBOX::SKYBOX(const IMAGE& image){
	Register(image);
}

void SKYBOX::Register(const IMAGE& image){
	//テクスチャを割り当てる
	texture.Assign(image, textureParams);

	//DisplayListに記録する
	{
		GL::DisplayList::Recorder recorder(displayList);
		GL::TEXTURE::BINDER binder(texture);
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

	//VIEWの描画対象にする
	VIEW::RegisterSkybox(*this);
}

void SKYBOX::Draw() const{
	displayList.Playback();
}

