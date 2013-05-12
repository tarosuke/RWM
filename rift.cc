#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <pthread.h>

extern "C"{
#include <OVR.h>
#include <OVR_HID.h>
}

#include "rift.h"


volatile bool RIFT::run(true);
double RIFT::rotation[16];
static Device* dev(0);

RIFT::RIFT(){
	//現在の方向をリセット
	mat4_identity(rotation);

	//Riftのセンサを準備
	if(!(dev = openRift(0,0))){
		printf("Could not locate Rift\n");
		printf("Be sure you have read/write permission to the proper /dev/hidrawX device\n");
		return;
	}

	printf("Device Info:\n");
	printf("\tname:     %s\n", dev->name);
	printf("\tlocation: %s\n", dev->location);
	printf("\tvendor:   0x%04hx\n", dev->vendorId);
	printf("\tproduct:  0x%04hx\n", dev->productId);

	//センサデータ取得開始
	pthread_t f1_thread;
	dev->runSampleThread = true;
	pthread_create(&f1_thread, NULL, RIFT::SensorThread, dev);
}

void RIFT::GetMatrix(double matrix[]){
	if(dev){
#if 0
		double m4[16];
		double rot[16];
		quat_toMat4(dev->Q, m4);
		mat4_toRotationMat(m4, rot);
		mat4_multiply(rotation, rot, rotation);
		mat4_set(dev->Q, matrix);
#else
		matrix[0] = dev->AngV[0];
		matrix[1] = dev->AngV[1];
		matrix[2] = dev->AngV[2];
#endif
//TODO:非同期に取得した値が一致するまで繰り返し取得、値が一致したら取得完了
	}
}

void* RIFT::SensorThread(void* initialData){
	Device& dev(*(Device*)initialData);
	fd_set readset;
	struct timeval waitTime;

	FD_ZERO(&readset);
	FD_SET(dev.fd,&readset);

	while(run){
		// 500ms
		waitTime.tv_sec = 0;
		waitTime.tv_usec = 500000;

		int result(select(
			dev.fd + 1, &readset, NULL, NULL, &waitTime));

		if (result && FD_ISSET( dev.fd, &readset )){
			sampleDevice(&dev);
		}

		// Send a keepalive - this is too often.  Need to only send on keepalive interval
		sendSensorKeepAlive(&dev);
	}
	return 0;
}
