#ifndef _PRIMITIVE_
#define _PRIMITIVE_

#include "q.h"


class PRIMITIVE : public Q<PRIMITIVE>::NODE{
	PRIMITIVE();
public:
	PRIMITIVE(Q<PRIMITIVE> q) : Q<PRIMITIVE>::NODE(q){}
	virtual void Draw() = 0;
private:
};


#endif
