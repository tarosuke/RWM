
#include <GL/glew.h>
#include <GL/gl.h>

#include "texture.h"
#include "../image/image.h"


namespace GL{

	TEXTURE::TEXTURE() : tid(0), empty(true){
		glGenTextures(1, const_cast<unsigned*>(&tid));
	}

	TEXTURE::TEXTURE(unsigned w, unsigned h) : tid(0), empty(true){
		glGenTextures(1, const_cast<unsigned*>(&tid));
	}

	TEXTURE::TEXTURE(const class IMAGE& image) : tid(0), empty(true){
		glGenTextures(1, const_cast<unsigned*>(&tid));
		Assign(image);
	}

	TEXTURE::~TEXTURE(){
		glDeleteTextures(1, &tid);
	}


	void TEXTURE::Bind() const{
		glBindTexture(GL_TEXTURE_2D, tid);
	}

	void TEXTURE::Assign(const IMAGE& image){
		empty = false;
	}

	void TEXTURE::Update(const IMAGE& image, int x, int y){
		if(empty){
			//テクスチャメモリが割り当てられていないので終了
			return;
		}
		Bind();
	}

}
