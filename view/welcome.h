#pragma once

#include "view.h"
#include "../gl/texture.h"
#include "../image/progressBar.h"


class WELCOME : public VIEW::DRAWER{
	WELCOME();
	WELCOME(const WELCOME&);
	void operator=(const WELCOME&);
public:
	WELCOME(VIEW&, const char* pngFile);
private:
	void Update();
	void Draw();
	float duration;
	static const float frameDuration = 1.0 / 75;
	GL::TEXTURE texture;
	bool finished;
	bool vanish;
	VIEW& view;
	float angle;
	PROGRESSBAR pBar;

	static const float width = 652.0 / 10000;
	static const float height = 1045.0 / 10000;
};

