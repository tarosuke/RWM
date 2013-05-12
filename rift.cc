#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <OVR.h>

#include "rift.h"


RIFT::RIFT(){
}

void RIFT::GetMatrix(double matrix[]){
}

void RIFT::SensorThread(void* initialData){
	Device& dev(*(Device*)initialData);
	struct pollfd riftFd = { dev.fd, POLLIN };
	fd_set readset;
	struct timeval waitTime;

	FD_ZERO(&readset);
	FD_SET(dev->fd,&readset);

	while(run){
		// 500ms
		waitTime.tv_sec = 0;
		waitTime.tv_usec = 500000;

		int result(select(
			dev->fd + 1, &readset, NULL, NULL, &waitTime));

		if (result && FD_ISSET( dev->fd, &readset )){
			sampleDevice(dev);
		}

		// Send a keepalive - this is too often.  Need to only send on keepalive interval
		sendSensorKeepAlive(dev);
	}
	return 0;
}
