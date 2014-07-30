#pragma once

#include "../display.h"


class XDISPLAY : public DISPLAY{
public:
	XDISPLAY();
	XDISPLAY(const char*);
	void Run();

private:

	static const int glxAttributes[];

	PARAM Setup();

	int damageBase;
	int dagame_err;
};


