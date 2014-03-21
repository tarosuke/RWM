#include <stdio.h>
#include <assert.h>

#include "ppm.h"


void PPM::ReadOne(char* buff, int maxLen){
	char* const tail(&buff[maxLen - 1]);
	do{
		*tail = 0xff;
		if(!fgets(buff, maxLen, (FILE*)file)){
			throw -1;
		}
	}while(buff[0] == '#' || ~*tail);
}


PPM::PPM(const char* path) : file(fopen(path, "r")){
	assert(file);

	char buff[128];
	//シグネチャ確認
	try{
		ReadOne(buff, sizeof(buff));
		if(buff[0] != 'P' || buff[1] != '6'){
			throw -1;
		}

		//サイズ取得
		unsigned w;
		unsigned h;
		ReadOne(buff, sizeof(buff));
		sscanf(buff, "%u%u", &w, &h);
		if(!w || !h){
			throw -1;
		}
		Allocate(w, h);

		//データ最大値取得
		ReadOne(buff, sizeof(buff));

		//本体取得
		if(1 != fread(image, w * h * 3, 1, (FILE*)file)){
			throw -1;
		}
	}
	catch(...){
		puts("faileh to load image.");
		Deallocate();
	}
	if(file){
		fclose((FILE*)file);
		file = 0;
	}
}
