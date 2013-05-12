#ifndef _WALL_
#define _WALL_

#include "primitive.h"


class WALL : public PRIMITIVE{
public:
	WALL();
	void Draw();
private:
	struct{
		GLfloat x;
		GLfloat y;
		GLfloat z;
	}vartexes[4];
};

// 法線を頂点ごとに設定できるようにしたWALL
// class CURVEDWALL : public PRIMITIVE{
// };


#endif
