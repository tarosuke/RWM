#pragma once

#include "../view.h"



class RIFT : public VIEW{
};


class RIFT_DK1 : public RIFT{
public:
	static VIEW* New();
private:
	RIFT_DK1(){};

	void PreDraw(){};
	void PostDraw(){};


	static const unsigned width = 1280;
	static const unsigned height = 800;
};