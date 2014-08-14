
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
		const unsigned d(image.Depth());
		glTexImage2D(
			GL_TEXTURE_2D, 0,
			d <= 24 ? GL_RGB : GL_RGBA,
			image.Width(), image.Height(), 0,
			d <= 24 ? GL_BGR : GL_BGRA,
			GL_UNSIGNED_BYTE, const_cast<void*>(image.Buffer()));
		empty = false;

		//属性を設定
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void TEXTURE::Update(const IMAGE& image, int x, int y){
		if(empty){
			//テクスチャメモリが割り当てられていないので終了
			return;
		}
		Bind();
	}

}
