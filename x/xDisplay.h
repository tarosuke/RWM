#pragma once

#include <X11/Xlib.h>



class XDISPLAY{
	XDISPLAY();
	XDISPLAY(const XDISPLAY&);
	void operator=(const XDISPLAY&);
public:
	XDISPLAY(unsigned w, unsigned h);
	XDISPLAY(const char*);
	void Run();

private:

	static const int glxAttributes[];



	
	Display* xDisplay;


	int damageBase;
	int dagame_err;
};


