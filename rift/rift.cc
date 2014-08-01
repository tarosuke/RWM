#include "rift.h"


FACTORY<VIEW> RIFT_DK1::factory(New);




VIEW* RIFT_DK1::New(){
	//TODO:デバイスファイルを確保してインスタンスをnew

	return new RIFT_DK1; //スタブ
}

