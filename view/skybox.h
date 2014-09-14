#pragma once


#include "view.h"
#include "../gl/texture.h"
#include "../gl/displayList.h"


class SKYBOX : VIEW::DRAWER{
	SKYBOX();
public:
	static SKYBOX* New(const char* path); //天箱生成(エラーはメッセージだけ)
	SKYBOX(const class IMAGE&); //展開図画像から生成
	SKYBOX(const char* path); //展開図画像のパスを指定
private:
	static const GL::TEXTURE::PARAMS textureParams;
	GL::TEXTURE texture;
	GL::DisplayList displayList;

	void Register(const class IMAGE&);
	void Draw();
};

