#pragma once

class IMAGE{
public:
	/** IMAGEから一部を取り出したIMAGEを作る
	 */
	IMAGE(const IMAGE&, int left, int top, int w, int h);
	unsigned GetWidth() const { return width; };
	unsigned GetHeight() const { return height; };
	const void* GetImage(int xOff = 0, int yOff = 0) const{
		return (void*)&((char*)image)[(yOff * width + xOff) * bpp]; };
	void* GetImageMem(int xOff = 0, int yOff = 0) const{
		return (void*)&((char*)image)[(yOff * width + xOff) * bpp]; };
	const void* GetMemoryImage() const{ return image; };
	virtual ~IMAGE();
protected:
	IMAGE() : image(0){};
	static const int bpp = 3;
	void* image;
	unsigned width;
	unsigned height;
	void Allocate(unsigned width_, unsigned height_);
	void Deallocate();
};

