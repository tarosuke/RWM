///ラスタイメージのラッパー

#pragma once



class Raster{
public:
	const void* Image(unsigned x = 0, unsigned y = 0) const{
		const char* const b((const char*)(buffer ? buffer : constBuffer));
		return &b[(depth / 8) * (width * y + x)]; };
	unsigned Size() const{ return width * height * depth; };
	unsigned Width() const{ return width; };
	unsigned Height() const{ return height; };
	unsigned Depth() const{ return depth; };
protected:
	Raster(unsigned w, unsigned h, unsigned d, const void* b) :
		width(w),
		height(h),
		depth(d),
		constBuffer(b),
		buffer(0){};
private:
	const unsigned width;
	const unsigned height;
	const unsigned depth;
	const void* const constBuffer;
	void* const buffer;
};



