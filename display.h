///////////////////////////////////////////////////////////// ABSTRACT DISPLAY
#pragma once

#include "view.h"


class DISPLAY{
	DISPLAY();
	DISPLAY(const DISPLAY&);
	void operator+(const DISPLAY&);

public:
	virtual void Run()=0;

protected:
	struct PARAM{
		unsigned width;
		unsigned height;
	};


	DISPLAY(const PARAM&);

private:
	VIEW& view;



	unsigned width;
	unsigned height;
};

