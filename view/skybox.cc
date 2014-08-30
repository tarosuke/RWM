///// 天箱

#include "../gl/gl.h"
#include "skybox.h"
#include "../image/image.h"
#include "../image/tga.h"


SKYBOX::SKYBOX(const char* path){
	TGAFile tga(path); //IMAGE側にファクトリっぽいものが必要
	texture.Assign(tga);

	VIEW::RegisterExternals(*this);
}

SKYBOX::SKYBOX(const IMAGE& image) : texture(image){
	VIEW::RegisterExternals(*this);
}


void SKYBOX::Record(){
}

void SKYBOX::Draw() const{
	//displayList.Playback();
}

