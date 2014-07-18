#pragma once

#include <GL/glew.h>
#include <GL/gl.h>


namespace GL{


	class Matrix{
	public:
		//Matrixをローカルで使うためのRAIIラッパ
		class Local{
		public:
			Local(Matrix& m){
				glPushMatrix();
				m.Multi();
			};
			Local(){
				glPushMatrix();
			};
			~Local(){
				glPopMatrix();
			};
			void Set(Matrix& m){
				m.Multi();
			};
		};

		Matrix(const float*);
		Matrix();
	private:
		float body[16];
		void Multi(){ glMultMatrixf(body); };
		void Load(){ glLoadMatrixf(body); };
	};


}


