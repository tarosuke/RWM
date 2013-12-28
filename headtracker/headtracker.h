/******************************************************** adstract headtracker
 *
 */
#pragma once

#include <toolbox/glpose/glpose.h>


class HEADTRACKER : public GLPOSE{
public:
	//TODO:HEADTRACKERそれ自体の構築子は隠してRIFTとかとのファクトリメソッドが必要
	HEADTRACKER(){};
	~HEADTRACKER(){};
protected:
private:
	GLPOSE pose;
};


