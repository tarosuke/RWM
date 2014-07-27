//////////////////////////////////////////////////////////////// ABSTRACT View
#pragma once


class View{
public:

	//環境をチェックして適切なViewを返す。引数は画面サイズ
	static View* New(unsigned width, unsigned height);
private:
};


