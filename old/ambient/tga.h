#pragma once


#include <raster.h>


class TGA : public Raster{
public:
	struct RAW{
		unsigned char IDLen;
		unsigned char IsColorMap;
		unsigned char type;
		unsigned short colorMapIndex;
		unsigned short colorMapLength;
		unsigned char colorMapEntrySize;
		unsigned short xOffset;
		unsigned short yOffset;
		unsigned short width;
		unsigned short height;
		unsigned char colorDepth;
		unsigned char attribute;
		unsigned char data[0]; //並び准はBGR(A)
	}__attribute__((packed));
	TGA(const void* rawTGA) : Raster(
		(*static_cast<const RAW*>(rawTGA)).width,
		(*static_cast<const RAW*>(rawTGA)).height,
		(*static_cast<const RAW*>(rawTGA)).colorDepth,
		(*static_cast<const RAW*>(rawTGA)).data){ Dump(rawTGA); };
private:

	void Dump(const void*);
};

