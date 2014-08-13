#include <stdlib.h>
#include <string.h>

#include "image.h"


IMAGE::IMAGE() :
	buffer(0),
	constBuffer(0),
	width(0),
	height(0),
	depth(0){}

IMAGE::IMAGE(const IMAGE& org) :
	buffer(malloc(org.Size())),
	constBuffer(0),
	width(org.Width()),
	height(org.Height()),
	depth(org.Depth()){
	memcpy(buffer, org.buffer ? org.buffer : org.constBuffer, Size());
}

IMAGE::IMAGE(const IMAGE& org, int x, int y, unsigned w, unsigned h) :
	buffer(malloc(w * h * org.Depth())),
	constBuffer(0),
	width(w),
	height(h),
	depth(org.Depth()){

	const char* const ob((const char*)(org.buffer ? org.buffer : org.constBuffer));
	char* b((char*)buffer);

	for(unsigned v(0); v < h; ++v){
		memcpy(&b[v * w * depth], &ob[((y + v) * org.width + x) * depth], w);
	}
}

IMAGE::IMAGE(const void* org, unsigned w, unsigned h, unsigned d) :
	buffer(0),
	constBuffer(org),
	width(w),
	height(h),
	depth(d){}

IMAGE::~IMAGE(){
	if(buffer){
		free(buffer);
	}
}

void IMAGE::operator=(const IMAGE& org){
	if(buffer){
		free(buffer);
	}
	constBuffer = 0;
	const unsigned s(org.Size());
	buffer = malloc(s);
	width = org.width;
	height = org.height;
	depth = org.depth;
	memcpy(buffer, org.buffer ? org.buffer : org.constBuffer, s);
}
