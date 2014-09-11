
#include <math.h>

#include "lightball.h"


LIGHTBALL::LIGHTBALL(unsigned size) : IMAGE(size, size, 4){
	//内容生成
	for(unsigned y(0); y < size; ++y){
		for(unsigned x(0); x < size; ++x){
			unsigned char* const px((unsigned char*)GetPoint(x, y));
			if(px){
				px[0] = px[1] = px[2] = 255;

				const float dx(x - 0.5 * size);
				const float dy(y - 0.5 * size);
//				const float t(255 - 511*sqrt(dx*dx+dy*dy) / size);
				const float t(32*size / sqrt(dx*dx+dy*dy) -48);
				px[3] = t < 0 ? 0 : 255 < t ? 255 : t;
			}
		}
	}
}