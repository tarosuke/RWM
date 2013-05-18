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

		(*userIn).Draw(remain);
	};
	ROOM();
	~ROOM();
private:
	static ROOM* userIn;
	static Q<ROOM> rooms;
	static const char* rcPath;
	Q<class PRIMITIVE> primitives;

	void Draw(int remain);
};


#endif
