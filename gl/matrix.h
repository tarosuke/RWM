#pragma once

namespace GL{


	class Matrix{
	public:
		//Matrixをローカルで使うためのRAIIラッパ
		class Local{
		public:
			Local(Matrix& m);
			Local();
			~Local();
		};

		Matrix(const float*);
		Matrix();
	private:
		float body[16];
		void Multi();
		void Load();
	};


}


