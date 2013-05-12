#include <stdlib.h>
#include <math.h>
#include <stdio.h>

extern "C"{
#include <OVR.h>
#include <OVR_HID.h>
}

#include "rift.h"


volatile bool RIFT::run(true);

RIFT::RIFT(){
}

void RIFT::GetMatrix(double matrix[]){
}

int RIFT::SensorThread(void* initialData){
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
