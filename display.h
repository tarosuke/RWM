///////////////////////////////////////////////////////////// ABSTRACT DISPLAY
#pragma once

#include "view.h"


class DISPLAY{
	DISPLAY(const DISPLAY&);
	void operator+(const DISPLAY&);

public:
	virtual void Run()=0;

protected:
	DISPLAY();

private:
	VIEW& view;
};

