#pragma once

#include "image.h"


class PNG : public IMAGE{
	PNG();
	PNG(const PNG&);
	void operator=(const PNG&);
public:
	PNG(const char* path);
private:
};
