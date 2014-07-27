///////////////////////////////////////////////////////////// ABSTRACT Display
#pragma once

#include "view.h"


class Display{
	Display();
	Display(const Display&);
	void operator+(const Display&);

public:
	virtual void Run()=0;

protected:
	Display(unsigned w, unsigned h);

private:
	View* view;
};

