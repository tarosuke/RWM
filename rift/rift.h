#pragma once

#include "../view.h"



class RIFT : public VIEW{
	RIFT();
	RIFT(const RIFT&);
	void operator=(const RIFT&);
public:
protected:
	RIFT(unsigned w, unsigned h) : VIEW(w, h){};
private:
};


class RIFT_DK1 : public RIFT{
public:
	static VIEW* New();
private:
	RIFT_DK1();

	void PreDraw(){};
	void PostDraw(){};


	static const unsigned width = 1280;
	static const unsigned height = 800;


	static FACTORY<VIEW> factory;
};
