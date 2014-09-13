#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <png.h>

#include "png.h"


PNG::PNG(const char* path){
	FILE* f(fopen(path, "rb"));
	void** rows(0);
	if(!f){
		throw "PNG:ファイルが開けない";
	}

	// png_struct
	png_structp png_ptr(png_create_read_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL));
	if(!png_ptr){
		fclose(f);
		throw "PNG:png構造を確保できない";
	}

	// PNGの情報
	png_infop info_ptr(png_create_info_struct(png_ptr));
	if(!info_ptr){
		png_destroy_read_struct(&png_ptr,NULL,NULL);
		fclose(f);
		throw "PNG:png情報(前)を取得できない";
	}
	png_infop end_info(png_create_info_struct(png_ptr));
	if(!end_info) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(f);
		throw "PNG:png情報(後)を取得できない";
	}
	//エラーハンドル設定
	if(setjmp(png_jmpbuf(png_ptr))){
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		if(rows){
			free(rows);
		}
		fclose(f);
		throw "PNG:読み込みに失敗";
	}

	//読み込みコールバックの設定
	png_init_io(png_ptr, f);

	// 画像情報の取得
	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;
	int interlace_type;
	int compression_type;
	int filter_type;

	png_read_info(png_ptr,info_ptr);
	png_get_IHDR(png_ptr,info_ptr,&width,&height,&bit_depth,&color_type,
		&interlace_type,&compression_type,&filter_type);

	//16/chanelなら8ビットにして取り出す
	if(16 == bit_depth){
		png_set_strip_16(png_ptr);
	}

//	png_size_t rowbytes=png_get_rowbytes(png_ptr,info_ptr);//行データのバイト数

	//チャンネル数取得
	const unsigned channels(png_get_channels(png_ptr, info_ptr));

	switch(channels){
	case 3 :
	case 4 :
		break;
	default:
		throw "PNG:サポートしていないチャネル数";
	}

	//画像メモリの割り当て
	AssignBuffer(width, height, channels);

	//各行の先頭を配列に
	rows = new void*[height];
	if(!rows){
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(f);
		throw "PNG:rowsの確保に失敗";
	}
	for(unsigned h(0); h < height; ++h){
		rows[h] = GetPoint(0, h);
	}

	//読み込み
	png_set_bgr(png_ptr);
	png_read_image(png_ptr, (png_bytepp)rows);

	//あとしまつ
	free(rows);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
}

