#pragma once

#include "view.h"


class WELCOME : public VIEW::DRAWER{
	WELCOME();
	WELCOME(const WELCOME&);
	void operator=(const WELCOME&);
public:
	WELCOME(const char* pngFile);
private:
	void Update();
	void Draw();
	float duration;
	static const float frameDuration = 1.0 / 75;
};

