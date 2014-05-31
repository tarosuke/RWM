#ifndef _PPM_
#define _PPM_

#include "image.h"

class PPM : public IMAGE{
public:
	PPM(const char* path);
	PPM(const void*);
	~PPM(){};
private:
	void* file;
	void ReadOne(char* buff, int maxLen);
};

#endif
