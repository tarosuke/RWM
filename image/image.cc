#include <stdlib.h>
#include <string.h>

#include "image.h"


IMAGE::~IMAGE(){
	if(image){
		free(image);
	}
}


void IMAGE::Allocate(unsigned width_, unsigned height_){
	if(image){
		free(image);
	}
	if(!(image = malloc(width_ * height_ * 3))){
		throw -1;
	}
	width = width_;
	height = height_;
}

void IMAGE::Deallocate(){
	if(image){
		free(image);
		image = 0;
	}
}


IMAGE::IMAGE(const IMAGE& org, int left, int top, int w, int h) : image(0){
	Allocate(w, h);

	for(int n(0); n < h; ++n){
		memcpy(GetImage(0, n), org.GetImage(left, top + n), bpp * width);
	}
}
