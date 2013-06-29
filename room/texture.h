#ifndef _TEXTURE_
#define _TEXTURE_

#include "image.h"

class TEXTURE{
	TEXTURE();
public:
	TEXTURE(const IMAGE& image);
	~TEXTURE();
	void Bind(unsigned id);
private:
	const unsigned size;
	const unsigned numOfTextures;
	unsigned texNames[32];
};

#endif
