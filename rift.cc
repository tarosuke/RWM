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

#include <GL/gl.h>

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

RIFT::RIFT() :
	fd(OpenDevice()),
	firstCycle(true),
	gravity(0.0, -9.8, 0.0){
	if(fd < 0){
		printf("Could not locate Rift\n");
		printf("sutup udev: SUBSYSTEM==\"hidraw\",ATTRS{idVendor}==\"2833\",ATTRS{idProduct}==\"0001\",MODE=\"0666\"\n");
		return;
	}

	//センサデータ取得開始
	pthread_create(&sensorThread, NULL, RIFT::_SensorThread, (void*)this);
}

RIFT::~RIFT(){
	pthread_cancel(sensorThread);
	close(fd);
}


void RIFT::GetView(){
	glLoadIdentity();
	if(IsEnable()){

		QON::ROTATION rotation;
		direction.GetRotation(rotation);
		glRotated(-rotation.angle * 180 / M_PI,
			rotation.x, rotation.y, rotation.z);

		glTranslated(-position.i, -position.j, -position.k);

// glPointSize(5);
// glBegin(GL_POINTS);
// glVertex3d(down.i, down.j, down.k);
// glEnd();
// glPointSize(3);
// glBegin(GL_POINTS);
// glVertex3d(gravity.i, gravity.j, gravity.k);
// glEnd();

	}
}


void RIFT::SensorThread(){
	fd_set readset;
	struct timeval waitTime;

	FD_ZERO(&readset);
	FD_SET(fd, &readset);

	for(;; pthread_testcancel()){
		// KeepAlive処理のために500msまでは待つ
		waitTime.tv_sec = 0;
		waitTime.tv_usec = 500000;

		int result(select(
			fd + 1, &readset, NULL, NULL, &waitTime));

		if(result && FD_ISSET( fd, &readset )){
			char buff[256];
			const int rb(read(fd, buff, 256));
			if(62 == rb){
				Decode(buff);
			}else{
				printf("%5d bytes dropped.\n", rb);
			}
		}

		// Send a keepalive - this is too often.  Need to only send on keepalive interval
		char buff[5];
		buff[0] = 8;
		buff[1] = buff[2] = 0; //command ID
		buff[3] = keepAliveInterval & 0xFF;
		buff[4] = keepAliveInterval >> 8;
		ioctl(fd, HIDIOCSFEATURE(5), buff);
	}
}
void* RIFT::_SensorThread(void* initialData){
	(*(RIFT*)initialData).SensorThread();
	return 0;
}


void RIFT::DecodeSensor(const unsigned char* buff, int v[3]){
	struct {int x:21;} s;

	v[0] = s.x =
		((unsigned)buff[0] << 13) |
		((unsigned)buff[1] << 5) |
		((buff[2] & 0xF8) >> 3);
	v[1] = s.x =
		(((unsigned)buff[2] & 0x07) << 18) |
		((unsigned)buff[3] << 10) |
		((unsigned)buff[4] << 2) |
		((buff[5] & 0xC0) >> 6);
	v[2] = s.x =
		(((unsigned)buff[5] & 0x3F) << 15) |
		((unsigned)buff[6] << 7) |
		(buff[7] >> 1);
}

void RIFT::Decode(const char* buff){
	struct{
		int accel[3];
		int rotate[3];
	}sample[3];
	int mag[3];

	//NOTE:リトルエンディアン機で動かす前提
	const unsigned char numOfSamples(buff[1]);
	const unsigned short timestamp(*(unsigned short*)&buff[2]);
	const short temp(*(short*)&buff[6]);

	const uint samples(numOfSamples > 2 ? 3 : numOfSamples);
	for(unsigned char i(0); i < samples; i++){
		DecodeSensor((unsigned char*)buff + 8 + 16 * i, sample[i].accel);
		DecodeSensor((unsigned char*)buff + 16 + 16 * i, sample[i].rotate);
	}
	mag[0] = *(short*)&buff[56];
	mag[1] = *(short*)&buff[58];
	mag[2] = *(short*)&buff[60];

	static unsigned short prevTime;
	const unsigned short deltaT(timestamp - prevTime);
	prevTime = timestamp;

	const float qtime(1.0/1000.0);
	temperature = 0.01 * temp;

	const double dt(qtime * deltaT / numOfSamples);

	// 各サンプル値で状況を更新
	for(unsigned char i(0); i < samples; i++){
		UpdateAngularVelocity(sample[i].rotate, dt);
		UpdateAccelaretion(sample[i].accel, dt);
	}

	// 磁界値の変換と向きの補正
	UpdateMagneticField(mag);

	firstCycle = false;
}


void RIFT::UpdateAngularVelocity(const int angles[3], double dt){
	QON delta(angles, 0.0001 * dt);
	direction *= delta;
}

void RIFT::UpdateAccelaretion(const int axis[3], double dt){
#if 1
	VQON accel(axis, 0.0001);
	accel.Rotate(direction); //地面系(のはず)へ変換
	accel.i = 0; //左右は無関係
	accel.Normalize();

	VQON down(0, -1, 0); //こうなっているはずの値

	//重力方向との差分で姿勢を補正
	QON differ(accel, down);
	differ *= 0.0001;
	direction *= differ;
#endif
}

void RIFT::UpdateMagneticField(const int axis[3]){
#if 0
	VQON north(axis, 1.0);
	north.Normalize();
	VQON n(0.0, 0.0, -1.0); //北
	n.ReverseRotate(direction);

	//北との差分で姿勢を補正
	QON differ(north, n);
	differ *= 0.00001;
	direction *= differ;
#endif
}

