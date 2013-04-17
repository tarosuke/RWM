#ifndef _ROOM_
#define _ROOM_

#include <GL/gl.h>

class ROOM{
public:
	static void Lead(const char* fileName);
	static void Update(){
		glPushMatrix();
		glColor3f(0.5, 0.5, 1.0);
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(-0.5, -1, -1);
		glVertex3f(0.5, -1, -1);
		glVertex3f(-0.5, 1, -1);
		glVertex3f(0.5, 1, -1);
		glEnd();
		glPopMatrix();
	};
};


#endif
