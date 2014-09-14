#include <string.h>

#include "progressBar.h"


PROGRESSBAR::PROGRESSBAR(unsigned width, unsigned height, float progress) :
	IMAGE(width, height, 3){

	//まずはクリア
	memset(WritableBuffer(), 0, Size());

	//progressまで塗る
	Update(progress);
}

void PROGRESSBAR::Update(float progress){
	//範囲正弦
	if(progress < 0.0){
		Error();
	}
	if(1.0 < progress){
		progress = 1.0;
	}

	for(unsigned y(0); y < Height(); ++y){
		for(unsigned x(0); x < Width() * progress; ++x){
			char* const p((char*)GetPoint(x, y));
			p[0] = p[1] = p[2] = 0x40; //TODO:仮の値
		}
	}
}

void PROGRESSBAR::Error(){
	for(unsigned y(0); y < Height(); ++y){
		for(unsigned x(0); x < Width(); ++x){
			char* const p((char*)GetPoint(x, y));
			p[0] = 0xff;
			p[1] = p[2] = 0;
		}
	}
}
