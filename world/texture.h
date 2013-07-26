#ifndef _TEXTURE_
#define _TEXTURE_

#include <image/image.h>


class TEXTURE{
public:
	TEXTURE() : loaded(false){};
	TEXTURE(const class IMAGE& image){
		Load(image);
	};
	~TEXTURE();
	void Load(const class IMAGE& image);
	void Bind(unsigned id) const;
private:
	static const unsigned maxNumOfTextures = 32;
	unsigned size;
	unsigned numOfTextures;
	unsigned texNames[maxNumOfTextures];
	bool loaded;
};

#endif
