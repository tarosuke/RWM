#include <GL/gl.h>
#include <stdio.h>
#include <assert.h>

#include "texset.h"


void TEXSET::Load(const IMAGE& image){
	//サイズとか数とか読み込む
	size = image.GetWidth();
	numOfTextures = image.GetHeight() / size;
	if(maxNumOfTextures < numOfTextures){
		numOfTextures = maxNumOfTextures;
	}

	//グラボ側でテクスチャの確保
	glEnable(GL_TEXTURE_2D);
	glGenTextures(numOfTextures, texNames);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//テクスチャイメージをグラボへ転送
	for(unsigned i(1); i <= numOfTextures; i++){
		glBindTexture(GL_TEXTURE_2D, texNames[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
			size, size, 0,
			GL_RGB, GL_UNSIGNED_BYTE,
			image.GetImage(0, size * (i - 1)));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	//一応後始末
	glBindTexture(GL_TEXTURE_2D, 0);

	//読み込んだよフラグ
	loaded = true;
}

TEXSET::~TEXSET(){
	if(loaded){
		glDeleteTextures(numOfTextures, texNames);
	}
}

void TEXSET::Bind(unsigned id) const{
	if(loaded){
		glBindTexture(GL_TEXTURE_2D,
			id < maxNumOfTextures ? texNames[id] : 0);
	}
}
