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

	const unsigned l(w / 4); //天箱の各辺の長さ[px]
	const unsigned ls(w * d); //一行のバイト数
	const char* src;
	char* dst;

	//「下」を上下逆にして二つ移動して「上」と並ばないようにする
	for(unsigned y(0); y < l; ++y){
		src = &b[((2 * l + y) * w + l) * d];
		dst = &b[((h - y) * w - 1) * d];
		for(unsigned x(0); x < l; ++x, src += d, dst -= d){
			memcpy(dst, src, d);
		}
	}

	//テクスチャアトラスの周辺を繋がるようにコピー
	//左上辺から左上右
	src = &b[l * ls];
	dst = &b[(l - 1) * d];
	for(unsigned n(0); n < l; ++n, src += d, dst += ls){
		*dst = *src;
	}
	//上左編から左上下
	src = &b[l * d];
	dst = &b[(l - 1) * ls];
	for(unsigned n(0); n < l; ++n, src += ls, dst += d){
		*dst = *src;
	}











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
	static const float p(500);
	static const float q(1.0 / 4);
	static const float t(1.0 / 3);
	static const float h[4] = { 0, q, 2*q, 3*q };
	static const float v[3] = { 0, t, 2*t };

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
	glTexCoord2f(4.0/4, v[1]);
	glVertex3f(-p, p, p);
	glTexCoord2f(4.0/4, v[2]);
	glVertex3f(-p, -p, p);
	glVertex3f(-p, -p, p);


	glTexCoord2f(h[3], v[2]);
	glVertex3f(p, -p, p);
	glVertex3f(p, -p, p);
	glTexCoord2f(h[3], 3.0/3);
	glVertex3f(p, -p, -p);
	glTexCoord2f(4.0/4, v[2]);
	glVertex3f(-p, -p, p);
	glTexCoord2f(4.0/4, 3.0/3);
	glVertex3f(-p, -p, -p);

	glEnd();
}

