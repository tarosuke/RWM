#ifndef _TEXTURE_
#define _TEXTURE_

#include "image.h"

class TEXTURE{
public:
	TEXTURE() : loaded(false){};
	~TEXTURE();
	void Load(const IMAGE& image);
	void Bind(unsigned id);
private:
	unsigned size;
	unsigned numOfTextures;
	unsigned texNames[32];
	bool loaded;
};

#endif
