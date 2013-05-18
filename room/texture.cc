#include <GL/gl.h>
#include <stdio.h>
#include <assert.h>

#include "texture.h"


TEXTURE::TEXTURE(const char* path) : PPM(path){
	printf("loading texture:%s...", path);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texNum);
assert(texNum);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, texNum);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		GetWidth(), GetHeight(), 0,
		GL_RGB, GL_UNSIGNED_BYTE, GetImage());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, 0);

	printf(image ? "OK(%dx%d).\n" : "FAILED.\n", GetWidth(), GetHeight());
}

TEXTURE::~TEXTURE(){
	glDeleteTextures(1, &texNum);
}

void TEXTURE::Bind(){
	glBindTexture(GL_TEXTURE_2D, texNum);
}
