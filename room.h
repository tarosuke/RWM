#ifndef _ROOM_
#define _ROOM_

#include <GL/gl.h>


class ROOM{
public:
	static void Load(const char* scriptFilename);
	static void Update(int remain){
		glEnable(GL_LIGHT0);
		const GLfloat v = 0.5;
		const GLfloat l[] = { v, v, v, 1 };
		const GLfloat p[] = { 0, 0, 0.1, 0 };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, l);
		glLightfv(GL_LIGHT0, GL_POSITION, p);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.5);
		glPushMatrix();
		glColor3f(0.5, 0.5, 1.0);
// 		glTranslatef(0, 0, 2);
		glBegin(GL_TRIANGLE_STRIP);
			glNormal3f(0, 0, 1);
			glVertex3f(-0.5, -1, -1);
			glVertex3f(0.5, -1, -1);
			glVertex3f(-0.5, 1, -1);
			glVertex3f(0.5, 1, -1);
		glEnd();

		glPopMatrix();
		glDisable(GL_LIGHT0);
	};
};


#endif
