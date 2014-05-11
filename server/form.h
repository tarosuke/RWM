///形状オブジェクト
#pragma once


#include <toolbox/queue.h>

class FORM{
public:
	virtual void Draw();
private:
	TOOLBOX::NODE<FORM> stockNode; //ストックのためのノード(TODO:ベクタへ修正)
	TOOLBOX::NODE<FORM> drawNode; //描画対象リストのためのノード
};

