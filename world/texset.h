#ifndef _TEXTURE_
#define _TEXTURE_

#include <image/image.h>


class TEXSET{
public:
	TEXSET() : loaded(false){};
	TEXSET(const class IMAGE& image){
		Load(image);
	};
	~TEXSET();
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
