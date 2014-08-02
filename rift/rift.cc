
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


FACTORY<VIEW> RIFT_DK1::factory(New);


VIEW* RIFT_DK1::New(){
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

		//確保完了
		printf("Oculus Rift DK1:%s.\n", name);
		return new RIFT_DK1(fd);
	}

	//なかった
	return 0;
}


#define MaxFloat (3.40282347e+38F)
#define G (9.80665)
RIFT_DK1::RIFT_DK1(int f) :
	RIFT(width, height),
	fd(f),
	averageRatio(initialAverageRatio),
	gravity((const double[]){ 0.0, -G, 0.0 }),
	magMax((const double[]){ -MaxFloat, -MaxFloat, -MaxFloat }),
	magMin((const double[]){ MaxFloat, MaxFloat, MaxFloat }),
	magFront((const double[]){ 0, 0, 1 }),
	magReady(false),
	magneticField((const double[3]){ 0.0, 0.0, 0.01 }){

#if 0
	//過去の磁化情報があれば取得
	settings.Fetch("magMax", &magMax);
	settings.Fetch("magMin", &magMin);
	settings.Fetch("magFront", &magFront);
#endif

	//スケジューリングポリシーを設定
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

	//デバイスを閉じないカーネルバグ対策で最初に一発Keepalive
	Keepalive();

	//センサデータ取得開始
	pthread_create(&sensorThread, &attr, _SensorThread, (void*)this);
}


RIFT_DK1::~RIFT_DK1(){
	pthread_cancel(sensorThread);
	pthread_join(sensorThread, 0);
	close(fd);

	//磁化情報を保存
#if 0
	settings.Store("magMax", &magMax);
	settings.Store("magMin", &magMin);
	settings.Store("magFront", &magFront);
#endif
}

void RIFT_DK1::Keepalive(){
	char buff[5];
	buff[0] = 8;
	buff[1] = buff[2] = 0; //command ID
	buff[3] = keepAliveInterval & 0xFF;
	buff[4] = keepAliveInterval >> 8;
	ioctl(fd, HIDIOCSFEATURE(5), buff);
}

void RIFT_DK1::SensorThread(){
	//優先度設定
	pthread_setschedprio(
		sensorThread,
		sched_get_priority_max(SCHED_FIFO));

	//Riftからのデータ待ち、処理
	fd_set readset;

	FD_ZERO(&readset);
	FD_SET(fd, &readset);

	for(;; pthread_testcancel()){
		int result(select(
			fd + 1, &readset, NULL, NULL, NULL));

		if(result && FD_ISSET( fd, &readset )){
			char buff[256];
			const int rb(read(fd, buff, 256));
			if(62 == rb){
				Decode(buff);
			}else{
				printf("%5d bytes dropped.\n", rb);
			}
		}

		//KeepAliveを送信
		Keepalive();
	}
}
void* RIFT_DK1::_SensorThread(void* initialData){
	//オブジェクトを設定して監視開始
	(*(RIFT_DK1*)initialData).SensorThread();
	return 0;
}


void RIFT_DK1::Decode(const char* buff){
}



