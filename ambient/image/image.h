#ifndef _IMAGE_
#define _IMAGE_

class IMAGE{
public:
	unsigned GetWidth() const { return width; };
	unsigned GetHeight() const { return height; };
	const void* GetImage(int xOff = 0, int yOff = 0) const{
		return (void*)&((char*)image)[(yOff * width + xOff) * bpp]; };
protected:
	IMAGE() : image(0){};
	~IMAGE();
	static const int bpp = 3;
	void* image;
	unsigned width;
	unsigned height;
	void Allocate(unsigned width_, unsigned height_);
	void Deallocate();
};

#endif
