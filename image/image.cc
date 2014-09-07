#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

	//TODO:もうちょっとマシなアルゴリズムに書き換える
	for(unsigned yd(0); yd < h; ++yd){
		for(unsigned xd(0); xd < w; ++xd){
			void* const d(GetPoint(xd, yd));
			const void* const s(org.GetConstPoint(x + xd, y + yd));
			if(d){
				if(s){
					memcpy(d, s, depth);
				}else{
					memset(d, 0, depth);
				}
			}
		}
	}
}

IMAGE::IMAGE(const void* org, unsigned w, unsigned h, unsigned d) :
	buffer(0),
	constBuffer(org),
	width(w),
	height(h),
	depth(d){}

IMAGE::IMAGE(unsigned w, unsigned h, unsigned d) :
	buffer(malloc(w * h * d)),
	constBuffer(0),
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


void IMAGE::AssignBuffer(void* newBuffer){
	if(buffer){
		free(buffer);
	}
	buffer = newBuffer;
}



bool IMAGE::IsInRange(unsigned x, unsigned y) const{
	return x < width && y < height;
}
unsigned IMAGE::GetOffset(unsigned x, unsigned y) const{
	return (y * width + x) * depth;
}
void* IMAGE::GetPoint(unsigned x, unsigned y){
	if(!IsInRange(x, y)){ return 0; }
	return (void*)((char*)buffer + GetOffset(x, y));
}
const void* IMAGE::GetConstPoint(unsigned x, unsigned y) const{
	if(!IsInRange(x, y)){ return 0; }
	return (const void*)((const char*)Buffer() + GetOffset(x, y));
}


void IMAGE::Update(const IMAGE& org, int x, int y){
	if(!buffer){
		throw "IMAGE:constなIMAGEをUpdateしようとした。";
	}


	//TODO:もっとマシなアルゴリズムに書き換える
	for(unsigned yy(0); yy < org.height; ++yy){
		for(unsigned xx(0); xx < org.width; ++xx){
			void* const d(GetPoint(xx + x, yy + y));
			const void* const s(org.GetConstPoint(xx, yy));
			if(d){
				if(s){
					memcpy(d, s, depth);
				}else{
					memset(d, 0, depth);
				}
			}
		}
	}
}
