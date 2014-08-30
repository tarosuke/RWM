
#include <GL/glew.h>
#include <GL/gl.h>

#include "texture.h"
#include "../image/image.h"


namespace GL{

	unsigned TEXTURE::BINDER::lastBinded(0);
	TEXTURE::BINDER::BINDER(const TEXTURE& t) : prevBinded(lastBinded){
//		if(lastBinded != t.tid){
			glBindTexture(GL_TEXTURE_2D, t.tid);
			lastBinded = t.tid;
//		}
	}
	TEXTURE::BINDER::~BINDER(){
//		if(prevBinded != lastBinded){
//			glBindTexture(GL_TEXTURE_2D, prevBinded);
//			lastBinded = prevBinded;
//		}
	}



	TEXTURE::TEXTURE() : tid(0), empty(true){
		glGenTextures(1, const_cast<unsigned*>(&tid));
	}

	TEXTURE::TEXTURE(unsigned w, unsigned h, bool a) : tid(0), empty(true){
		glGenTextures(1, const_cast<unsigned*>(&tid));
		BINDER b(*this);
		glTexStorage2D(GL_TEXTURE_2D, 0, a ? GL_RGBA : GL_RGB, w, h);
		SetupAttributes();
		empty = false;
	}

	TEXTURE::TEXTURE(const class IMAGE& image) : tid(0), empty(true){
		glGenTextures(1, const_cast<unsigned*>(&tid));
		Assign(image);
	}

	TEXTURE::~TEXTURE(){
		glDeleteTextures(1, &tid);
	}

	void TEXTURE::Assign(const IMAGE& image){
		BINDER b(*this);

		const unsigned d(image.Depth());
		glTexImage2D(
			GL_TEXTURE_2D, 0,
			d <= 24 ? GL_RGB : GL_RGBA,
			image.Width(), image.Height(), 0,
			d <= 24 ? GL_BGR : GL_BGRA,
			GL_UNSIGNED_BYTE, const_cast<void*>(image.Buffer()));
		empty = false;

		//属性を設定
		SetupAttributes();
	}

	void TEXTURE::SetupAttributes(){
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}

	void TEXTURE::Update(const IMAGE& image, int x, int y){
		if(empty){
			//テクスチャメモリが割り当てられていないので終了
			return;
		}
		BINDER b(*this);
		glTexSubImage2D(
			GL_TEXTURE_2D, 0,
			x,
			y,
			image.Width(),
			image.Height(),
			GL_BGRA,
			GL_UNSIGNED_BYTE,
			image.Buffer());
	}

}
