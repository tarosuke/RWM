
#include "view.h"
#include "rift/rift.h"



VIEW& VIEW::New() throw(const char*){
	VIEW* v;

	if((v = RIFT_DK1::New())){
		return *v;
	}

	throw "VRHMDが見つかりませんでした";
}
