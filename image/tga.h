#pragma once


#include "image.h"


class TGA : public IMAGE{
	TGA();
	TGA(const TGA&);
	void operator=(const TGA&);
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
	TGA(const void* rawTGA) : IMAGE(
		(*static_cast<const RAW*>(rawTGA)).data,
		(*static_cast<const RAW*>(rawTGA)).width,
		(*static_cast<const RAW*>(rawTGA)).height,
		((*static_cast<const RAW*>(rawTGA)).colorDepth + 7) >> 3){
			Dump(rawTGA); };
private:

	static RAW& Map(const char*);
	static void Dump(const void*);
};

class TGAFile : public TGA{
	TGAFile();
	TGAFile(const TGAFile&);
	void operator=(const TGAFile&);
public:
	TGAFile(const char* path) : TGA((void*)Map(path)){};
	~TGAFile();
private:
	int fd;
	const RAW* raw;
	unsigned len;

	const RAW* Map(const char*)throw(const char*);
};

