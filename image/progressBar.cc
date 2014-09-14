#include <string.h>

#include "progressBar.h"


const unsigned char PROGRESSBAR::barColor[] = { 0x80, 0xff, 0 };
const unsigned char PROGRESSBAR::white[] = { 0xff, 0xff, 0xff };
const unsigned char PROGRESSBAR::errorColor[] = { 0, 0, 0xff };


PROGRESSBAR::PROGRESSBAR(unsigned width, unsigned height, float progress) :
	IMAGE(width, height, 3){

	//まずはクリア
	memset(WritableBuffer(), 0, Size());

	//progressまで塗る
	Update(progress);
}

void PROGRESSBAR::Update(float progress){
	//範囲正弦
	unsigned w(Width()*progress);
	const unsigned char* c(barColor);
	c = barColor;
	if(progress < 0.0 || 1.0 < progress){
		w = Width();
		c = errorColor;
	}

	for(unsigned y(0); y < Height(); ++y){
		for(unsigned x(0); x < w; ++x){
			unsigned char* const p((unsigned char*)GetPoint(x, y));
			memcpy(p, 4 == y ? white : c, Depth());
		}
	}
}
