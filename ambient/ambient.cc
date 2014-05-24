

#include <GL/glew.h>
#include <GL/gl.h>

#include <ambient.h>


Ambient::Ambient(VIEW& v){
	v.RegisterExternals(*this);
}



void Ambient::Draw() const{
	glBegin(GL_POLYGON);
	glVertex3f(0, 2.4-1.6, -5);
	glVertex3f(-1, -1.6, -5);
	glVertex3f(1, -1.6, -5);
	glEnd();
}

