#include "rift.h"


VIEW* (*RIFT::factories[])() ={
	RIFT_DK1::New,
	0,
};


VIEW* RIFT::New(){
	for(VIEW* (**f)()(factories); *f; ++f){
		VIEW* const v((**f)());
		if(v){
			return v;
		}
	}
	return 0;
}




VIEW* RIFT_DK1::New(){
	//TODO:デバイスファイルを確保してインスタンスをnew

	return new RIFT_DK1; //スタブ
}

