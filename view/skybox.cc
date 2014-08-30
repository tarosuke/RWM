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

void SKYBOX::Register(const IMAGE& org){
	//orgはconstなので修正するためにコピー
	IMAGE image(org);
	const unsigned w(image.Width());
	const unsigned h(image.Height());
	const unsigned d(image.Depth());
	char* const b((char*)image.WritableBuffer());
assert(b);
	//「下」を上下逆にして左へ二つ移動
	for(unsigned y(0); y < h / 3; ++y){
		const char* src(&b[(((2 * h) / 3 + y) * w + w / 4) * d]);
		char* dst(&b[((h - y) * w - 1) * d]);
		for(unsigned x(0); x < w / 4; ++x, src += d, dst -= d){
			memcpy(dst, src, d);
		}
	}

	//テクスチャアトラスの周辺を繋がるようにコピー














	//テクスチャを割り当てる
	texture.Assign(image);

	//TODO:DisplayListに記録する
	Record();

	//VIEWの描画対象にする(暫定)
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


	glTexCoord2f(3.0/4, 2.0/3);
	glVertex3f(p, -p, p);
	glVertex3f(p, -p, p);
	glTexCoord2f(3.0/4, 3.0/3);
	glVertex3f(p, -p, -p);
	glTexCoord2f(4.0/4, 2.0/3);
	glVertex3f(-p, -p, p);
	glTexCoord2f(4.0/4, 3.0/3);
	glVertex3f(-p, -p, -p);

	glEnd();
}

