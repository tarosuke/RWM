#include "../gl/gl.h"
#include "reference.h"

void REFERENCE::DrawAll(){
	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	for(float n(-200); n <= 200; n += 10){
		//縦
		glVertex3f(n, -200, -100);
		glVertex3f(n, 200, -100);
		//横
		glVertex3f(-200, n, -100);
		glVertex3f(200, n, -100);
	}
	glEnd();
}
