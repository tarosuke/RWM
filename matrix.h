#ifndef _MATRIX_
#define _MATRIX_

#include <math.h>

template<int dimensions> class MATRIX{
public:
	void operator+=(const MATRIX& o){
		for(int i(0); i < dimensions; i++){
			for(int j(0); j < dimensions; j++){
				matrix[i][j] += o.matrix[i][j];
			}
		}
	};
	void operator-=(const MATRIX& o){
		for(int i(0); i < dimensions; i++){
			for(int j(0); j < dimensions; j++){
				matrix[i][j] -= o.matrix[i][j];
			}
		}
	};
	void operator*=(const MATRIX& o){
		MATRIX newMatrix;
		for(int i(0); i < dimensions; i++){
			for(int j(0); j < dimensions; j++){
				float& t(newMatrix.matrix[i][j]);
				for(int k(0); k < dimensions; k++){
					t += matrix[i][k] * o.matrix[k][j];
				}
			}
		}
		*this = newMatrix;
	};
	//TODO:rotateとかmoveとかscaleとか
private:
	float matrix[dimensions][dimensions];
};

#endif
