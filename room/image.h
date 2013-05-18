#ifndef _IMAGE_
#define _IMAGE_

class IMAGE{
public:
	unsigned GetWidth(){ return width; };
	unsigned GetHeight(){ return height; };
	void* GetImage(){ return image; };
protected:
	IMAGE() : image(0){};
	~IMAGE();
	void* image;
	unsigned width;
	unsigned height;
	void Allocate(unsigned width_, unsigned height_);
	void Deallocate();
};

#endif
