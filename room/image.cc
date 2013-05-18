#include <stdlib.h>

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
