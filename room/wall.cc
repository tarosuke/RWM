#include <GL/gl.h>

#include "wall.h"



void WALL::Draw(){
	glBegin(GL_QUADS);
	for(int i(0); i < 4; i++){
		glVertex3fv((GLfloat*)&vartexes[i]);
	}
	glEnd();
}
