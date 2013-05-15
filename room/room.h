#ifndef _ROOM_
#define _ROOM_

#include <assert.h>

#include "q.h"

#include <GL/gl.h>


class ROOM : public Q<ROOM>::NODE{
public:
	static void Load(bool test);
	static void Unload();
	static void Update(int remain){
		assert(userIn);

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
			glVertex3f(-0.5, -1, far);
			glVertex3f(0.5, -1, far);
			glVertex3f(-0.5, 1, far);
			glVertex3f(0.5, 1, far);
		glEnd();

		glPopMatrix();
		glDisable(GL_LIGHT0);

		if((far <= -100.0 && dir < 0.0) || (-1.0 <= far && 0.0 < dir)){
			dir = -dir;
		}
		far += dir;
	};
	ROOM();
	~ROOM();
private:
	static float far;
	static float dir;

	static ROOM* userIn;
	static Q<ROOM> rooms;
	static const char* rcPath;
	Q<class PRIMITIVE> primitives;
};


#endif
