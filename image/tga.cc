#include "tga.h"

#include <stdio.h>


void TGA::Dump(const void* rawTga){
	const RAW& r(*(const RAW*)rawTga);
	printf("IDLen:%d.\n", r.IDLen);
	printf("IsColorMap:%d.\n", r.IsColorMap);
	printf("type:%d.\n", r.type);
	printf("colorMapIndex:%d.\n", r.colorMapIndex);
	printf("colorMapLength:%d.\n", r.colorMapLength);
	printf("colorMapEntrySize:%d.\n", r.colorMapEntrySize);
	printf("xOffset:%d.\n", r.xOffset);
	printf("yOffset:%d.\n", r.yOffset);
	printf("width:%d.\n", r.width);
	printf("height:%d.\n", r.height);
	printf("colorDepth:%d.\n", r.colorDepth);
	printf("attribute:%x.\n", r.attribute);
}

