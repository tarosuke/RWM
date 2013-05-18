#ifndef _TEXTURE_
#define _TEXTURE_

#include "ppm.h"

class TEXTURE : private PPM{
public:
	TEXTURE(const char* path);
	~TEXTURE();
	void Bind();
private:
	unsigned texNum;
};

#endif
