#include <string.h>

#include "matrix.h"



namespace GL{

	Matrix::Zone::Zone(Matrix& m){
		glPushMatrix();
		glMultMatrixf(m.matrix);
	};
	Matrix::Zone::Zone(Matrix& m, FLAG){
		glPushMatrix();
		glLoadMatrixf(m.matrix);
	};
	Matrix::Zone::~Zone(){
		glPopMatrix();
	}


	Matrix::Matrix(){
		for(unsigned n(0); n < 16; ++n){
			matrix[n] = 0;
		}
		matrix[0] = matrix[5] = matrix[10] = 1;
	}
	Matrix::Matrix(const float body[16]){
		memcpy(matrix, body, sizeof(matrix));
	}

}
