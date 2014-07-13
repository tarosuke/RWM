#include <GL/glew.h>
#include <GL/gl.h>

#include <string.h>

#include "matrix.h"



namespace GL{

	Matrix::Matrix(const float* m){
		memcpy(body, m, sizeof(body));
	}

	void Matrix::Multi(){
		glMultMatrixf(body);
	}
	void Matrix::Load(){
		glLoadMatrixf(body);
	}


	Matrix::Matrix(){
		for(unsigned n(0); n < 16; ++n){
			body[n] = 0.0;
		}
		body[0] =
		body[5] =
		body[10] = 1;
	}


	Matrix::Local::Local(Matrix& m){
		glPushMatrix();
		m.Multi();
	}

	Matrix::Local::Local(){
		glPushMatrix();
	}

	Matrix::Local::~Local(){
		glPopMatrix();
	}

}
