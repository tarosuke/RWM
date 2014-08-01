
#include "view.h"
#include "rift/rift.h"



template<> FACTORY<VIEW>* FACTORY<VIEW>::start(0);


VIEW& VIEW::New() throw(const char*){
	VIEW* const v(FACTORY<VIEW>::New());
	if(v){
		return *v;
	}

	throw "VRHMDが見つかりませんでした";
}
