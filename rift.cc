#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/file.h>
#include <assert.h>

#include "rift.h"


volatile bool RIFT::run(true);
Device* RIFT::dev(0);
Device RIFT::device;

RIFT::RIFT(){
	assert(!dev);

	//Riftのセンサを準備
	for(int i(0); i < 99; i++){
		char name[32];
		snprintf(name, 32, "/dev/hidraw%d", i);
		const int fd(open(name, O_RDWR | O_NONBLOCK));
		if(fd < 0){
			//開けなかった
			continue;
		}

		struct hidraw_devinfo info;
		if(ioctl(fd, HIDIOCGRAWINFO, &info) < 0){
			//ioctlできない=riftではない
			close(fd);
			continue;
		}
		if(VendorID != info.vendor || ProductID != info.product){
			//riftではない
			close(fd);
			continue;
		}

		if(flock(fd, LOCK_EX | LOCK_NB) < 0){
			//使用中
			close(fd);
			continue;
		}

		//この時点でのfdがrift
		device.fd = fd;
		dev = &device;
		getDeviceInfo(dev);
		break;
	}
	if(!dev){
		printf("Could not locate Rift\n");
		printf("Be sure you have read/write permission to the proper /dev/hidrawX device\n");
		return;
	}

	//devの初期化
	dev->keepAliveIntervalMs = 1000;
	dev->Coordinates = Coord_Sensor;
	dev->HWCoordinates = Coord_HMD;
	dev->Gain = 0.5;
	dev->YawMult = 1.0;
	dev->EnablePrediction = FALSE;
	dev->EnableGravity = TRUE;
	dev->Q[3] = 1.0;

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
#if 1
		double m4[16];
		quat_toMat4(dev->Q, m4);
		mat4_toRotationMat(m4, matrix);
#else
		matrix[0] = dev->AngV[0];
		matrix[1] = dev->AngV[1];
		matrix[2] = dev->AngV[2];
#endif
	}else{
		mat4_identity(matrix);
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
