#pragma once

#include <GL/glew.h>
#include <GL/gl.h>


namespace GL{

	class Matrix{
	public:
		//MatrixのRAIIラッパ
		class Zone{
			Zone();
		public:
			enum FLAG{ load };
			Zone(const Matrix& m);
			Zone(const Matrix& m, FLAG);
			~Zone();
		};

		Matrix(); //初期値は単位行列
		Matrix(const float body[16]);
	private:
		float matrix[16];
	};


}


