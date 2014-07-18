#include <string.h>

#include "matrix.h"



namespace GL{

	Matrix::Matrix(const float* m){
		memcpy(body, m, sizeof(body));
	}

}
