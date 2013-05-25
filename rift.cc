#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/file.h>
#include <assert.h>

#include "rift.h"


int RIFT::OpenDevice(){
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
		return fd;
	}
	return -1;
}

RIFT::RIFT() : fd(OpenDevice()), dev(0), run(true){
	if(fd < 0){
		run = false;
		printf("Could not locate Rift\n");
		printf("sutup udev: SUBSYSTEM==\"hidraw\",ATTRS{idVendor}==\"2833\",ATTRS{idProduct}==\"0001\",MODE=\"0666\"\n");
		return;
	}

	//デバイスファイルは準備できている
	dev = &device;

	//devの初期化
	dev->keepAliveIntervalMs = 1000;
	dev->Coordinates = Coord_Sensor;
	dev->HWCoordinates = Coord_HMD;
	dev->Gain = 0.5;
	dev->YawMult = 1.0;
	dev->EnablePrediction = FALSE;
	dev->EnableGravity = TRUE;
	dev->Q[3] = 1.0;

	//センサデータ取得開始
	pthread_t f1_thread;
	dev->runSampleThread = true;
	pthread_create(&f1_thread, NULL, RIFT::_SensorThread, (void*)this);
}

void RIFT::GetMatrix(double matrix[]){
	if(dev){
#if 1
		double m4[16];
		double q[4];
		q[0] = (*dev).Q[0];
		q[1] = -(*dev).Q[1];
		q[2] = -(*dev).Q[2];
		q[3] = -(*dev).Q[3];
		quat_toMat4(q, m4);
		mat4_toRotationMat(m4, matrix);
		for(int i(0); i < 4; i++){
			double* row = &matrix[i * 4];
			row[1] = -row[1];
			const double t(row[1]);
			row[1] = row[2];
			row[2] = t;

			if(~i & 1){
				for(int j(0); j < 4; j++){
					row[j] = -row[j];
				}
			}
		}
#else
		matrix[0] = dev->AngV[0];
		matrix[1] = dev->AngV[1];
		matrix[2] = dev->AngV[2];
#endif
	}else{
		mat4_identity(matrix);
	}
}


void RIFT::SensorThread(){
	fd_set readset;
	struct timeval waitTime;

	FD_ZERO(&readset);
	FD_SET(fd, &readset);

	while(run){
		// KeepAlive処理のために500msまでは待つ
		waitTime.tv_sec = 0;
		waitTime.tv_usec = 500000;

		int result(select(
			fd + 1, &readset, NULL, NULL, &waitTime));

		if(result && FD_ISSET( fd, &readset )){
			char buff[256];
			const int rv(read(fd, buff, 256));
			switch(rv){
			case 62 * 4 :
			case 62 * 3 :
			case 62 * 2 :
			case 62 :
				for(int n(0); n < rv; n+= 62){
					Decode(&buff[n]);
				}
				break;
			}
		}

		// Send a keepalive - this is too often.  Need to only send on keepalive interval
		{
			char buff[5];
			buff[0] = 8;
			buff[1] = buff[2] = 0; //command ID
			buff[3] = keepAliveInterval & 0xFF;
			buff[4] = keepAliveInterval >> 8;
			ioctl(fd, HIDIOCSFEATURE(5), buff);
		}
	}
}
void* RIFT::_SensorThread(void* initialData){
	(*(RIFT*)initialData).SensorThread();
	return 0;
}


void RIFT::DecodeSensor(const char* buff, int* const v){
	struct {int x:21;} s;

	v[0] = s.x = (buff[0] << 13) | (buff[1] << 5) | ((buff[2] & 0xF8) >> 3);
	v[1] = s.x = ((buff[2] & 0x07) << 18) | (buff[3] << 10) | (buff[4] << 2) | ((buff[5] & 0xC0) >> 6);
	v[2] = s.x = ((buff[5] & 0x3F) << 15) | (buff[6] << 7) | (buff[7] >> 1);
}

void RIFT::Decode(const char* buff){
	unsigned char numOfSamples;
	unsigned short timestamp;
	unsigned short lastCommandID;
	short temperature;
	struct{
		int accel[3];
		int rotate[3];
	}sample[3];
	short mag[3];

	//NOTE:リトルエンディアン機で動かす前提
	numOfSamples = buff[1];
	timestamp = *(unsigned short*)&buff[2];
	lastCommandID = *(unsigned short*)&buff[4];
	temperature = *(short*)&buff[6];

	const uint samples(numOfSamples > 2 ? 3 : numOfSamples);
	for(unsigned char i(0); i < samples; i++){
		DecodeSensor(buff + 8 + 16 * i, sample[i].accel);
		DecodeSensor(buff + 16 + 16 * i, sample[i].rotate);
	}
	mag[0] = *(short*)&buff[56];
	mag[1] = *(short*)&buff[58];
	mag[2] = *(short*)&buff[60];

#if 0
	const float timeq(1.0/1000.0);
	double Acceleration[3];   // 加速度[m/s^2]
	double RotationRate[3];   // 角速度[rad/s^2]
	double MagneticField[3];  // 磁力[Gauss]
	float Temperature;    // センサ表面温度[℃]
	float TimeDelta;      // 前回レポートからの経過時間[病]

	lastSampleCount = sampleCount;
	lastTimestamp = timestamp;
#else
	//libovrを使用
	TrackerSensors s;
	s.SampleCount = samples;
	s.Timestamp = timestamp;
	s.LastCommandID = lastCommandID;
	s.Temperature = temperature;
	for(unsigned char i(0); i < samples; i++){
		s.Samples[i].AccelX = sample[i].accel[0];
		s.Samples[i].AccelY = sample[i].accel[1];
		s.Samples[i].AccelZ = sample[i].accel[2];
		s.Samples[i].GyroX = sample[i].rotate[0];
		s.Samples[i].GyroY = sample[i].rotate[1];
		s.Samples[i].GyroZ = sample[i].rotate[2];
	}
	s.MagX = mag[0];
	s.MagY = mag[1];
	s.MagZ = mag[2];

	processTrackerData(dev, &s);
#endif
}

