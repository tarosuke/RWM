#include "tga.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>


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

const TGA::RAW* TGAFile::Map(const char* path) throw(const char*){
	fd = open(path, O_RDONLY);
	if(fd < 0){
		throw "TGAFile:ファイルが開けなかった。";
	}

	struct stat stat;
	fstat(fd, &stat);
	len = stat.st_size;

	raw = (const RAW*)mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0);
	if(!raw){
		throw "TGAFile:ファイルをマップできなかった。";
	}

	//サイズチェック
	const unsigned mayBeSize(
		(*raw).width * (*raw).height * (*raw).colorDepth/8 + sizeof(RAW));
	if(len < mayBeSize || mayBeSize * 2 < len){
		printf("size:%u mayBe:%u.\n", len, mayBeSize);
		throw "TGAFile:ファイルサイズに異常がある。";
	}

	return raw;
}

TGAFile::~TGAFile(){
	if(0 <= fd){
		close(fd);
	}
	if(raw){
		munmap(const_cast<void*>((const void*)raw), len);
	}
}
