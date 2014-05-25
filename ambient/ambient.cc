

#include <GL/glew.h>
#include <GL/gl.h>

#include <ambient.h>


Ambient::Ambient(VIEW& v){
	v.RegisterExternals(*this);
}



void Ambient::Draw() const{
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

	//固定シェーダの照明は頂点単位なのであんまり意味がないので使わない
	glDisable(GL_LIGHTING);



	glColor4f(0.7, 0.7, 1.0, 1);

	static const float vArr[] = {
		-2, 2.4-1.6, 2,
		-2, -1.6, 2,
		-2, 2.4-1.6, -4,
		-2, -1.6, -4};
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vArr);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(vArr) / sizeof(vArr[0]));
}

