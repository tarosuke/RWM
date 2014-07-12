#include <stdio.h>
#include <assert.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <ambient.h>
#include <ambient/tga.h>


Ambient::Room* Ambient::in(0);
TOOLBOX::QUEUE<Ambient::Room> Ambient::rooms;
unsigned Ambient::sequence(0);



extern "C" const TGA::RAW _binary_texture_tga_start;


Ambient::Ambient(VIEW& v) : texSet(TGA(&_binary_texture_tga_start)){
	v.RegisterExternals(*this);

// 	in = new SquareRoom(4,6);
}






Ambient::TexSet::TexSet(const Raster& r) :
	size(r.Width()),
	numOf(r.Height() / size),
	ids(new unsigned[numOf]){
	//テクスチャ準備
	assert(ids);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(numOf, ids);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//テクスチャ読み込み
	for(unsigned n(0); n < numOf; ++n){
		glBindTexture(GL_TEXTURE_2D, ids[n]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
			size, size, 0,
			GL_RGB, GL_UNSIGNED_BYTE,
			r.Image(0, size * n));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	//一応後始末
	glBindTexture(GL_TEXTURE_2D, 0);
}

Ambient::TexSet::~TexSet(){
	if(ids){
		glDeleteTextures(numOf, ids);
	}
}

void Ambient::TexSet::Activate(unsigned id) const{
	glBindTexture(GL_TEXTURE_2D, ids[id]);
}

void Ambient::TexSet::Deactivate() const{
	glBindTexture(GL_TEXTURE_2D, 0);
}








void Ambient::Draw() const{
	++sequence;

	//ジオメトリエンジンを動かすのはこのタイミング

	//フォグ(天候の演出とかで使う予定だけどまだ使わない)
#if 0
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogi(GL_FOG_START, 10);
	glFogi(GL_FOG_END, 5000);
	const GLfloat fogColor[] = { 0.6, 0.6, 0.6, 0.6 };
	glFogfv(GL_FOG_COLOR, fogColor);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glEnable(GL_FOG);
#endif

// 	glEnable(GL_CULL_FACE);
// 	glFrontFace(GL_CW);

	glEnable(GL_TEXTURE_2D);

	if(in){
		(*in).Draw(texSet, 1);
	}
	glDisable(GL_CULL_FACE);
}





