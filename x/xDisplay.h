#pragma once

#include "../display.h"


class XDISPLAY : public DISPLAY{
public:
	XDISPLAY();
	void Run();

private:

	static const int glxAttributes[];
};


