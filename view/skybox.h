#pragma once


#include "view.h"
#include "../gl/texture.h"
#include "../gl/displayList.h"


class SKYBOX : VIEW::DRAWER{
	SKYBOX();
public:
	SKYBOX(const class IMAGE&); //展開図画像から生成
	SKYBOX(const char* path); //展開図画像のパスを指定
private:
	GL::TEXTURE texture;
	GL::DisplayList displayList;

	void Register(const class IMAGE&);
	void Draw() const;
	void Record();
};

