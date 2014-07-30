
#include "view.h"
#include "rift/rift.h"



VIEW& VIEW::New(unsigned w, unsigned h) throw(const char*){
	VIEW* v;

	if((v = RIFT_DK1::New())){
		return *v;
	}

	throw "VRHMDが見つかりませんでした";
}
