#pragma once



class Raster{
protected:
	Raster(unsigned w, unsigned h, unsigned d, const void* b) :
		width(w),
		height(h),
		depth(d),
		buffer(b),
		heapBuffer(false){};
private:
	const unsigned width;
	const unsigned height;
	const unsigned depth;
	const void* const buffer;
	const bool heapBuffer;
};




class TGA : public Raster{
public:
	TGA(const void* rawTGA) : Raster(
		(*static_cast<const RAW*>(rawTGA)).width,
		(*static_cast<const RAW*>(rawTGA)).height,
		(*static_cast<const RAW*>(rawTGA)).colorDepth,
		(*static_cast<const RAW*>(rawTGA)).data){};
private:
	struct RAW{
		unsigned char IDLen;
		unsigned char IsColorMap;
		unsigned char type;
		unsigned short colorMapEntry;
		unsigned char colorMapEntrySize;
		unsigned short xOffset;
		unsigned short yOffset;
		unsigned short width;
		unsigned short height;
		unsigned char colorDepth;
		unsigned char attribute;
		unsigned char data[0]; //並び准はBGR(A)
	}__attribute__((packed));
};

