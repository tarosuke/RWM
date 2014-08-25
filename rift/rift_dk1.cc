
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <assert.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

#include "rift.h"
#include "../settings/settings.h"


FACTORY<VIEW> RIFT_DK1::factory(New);


VIEW* RIFT_DK1::New(){
	const int fd(OpenDeviceFile(ProductID));
	if(0 <= fd){
		//確保完了
		puts("Oculus Rift DK1 found.\n");
		return new RIFT_DK1(fd);
	}

	//なかった
	return 0;
}


RIFT_DK1::RIFT_DK1(int f) : RIFT(f, width, height){}


RIFT_DK1::~RIFT_DK1(){
}
