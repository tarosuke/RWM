#include <stdio.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <ambient.h>


Ambient::Room* Ambient::in(0);
TOOLBOX::QUEUE<Ambient::Room> rooms;
unsigned Ambient::sequence(0);


Ambient::Ambient(VIEW& v){
	v.RegisterExternals(*this);

	in = new SquareRoom(4,6);
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
// 	glFrontFace(GL_CCW);

	if(in){
		(*in).Draw(1);
	}
	glDisable(GL_CULL_FACE);
}

