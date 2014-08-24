
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


RIFT_DK1::RIFT_DK1(int f) :
	RIFT(f, width, height){

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

void RIFT_DK1::DecodeSensor(const unsigned char* buff, int v[3]){
	struct {int x:21;} s;

	v[0] = s.x =
		((unsigned)buff[0] << 13) |
		((unsigned)buff[1] << 5) |
		((buff[2] & 0xfb) >> 3);
	v[1] = s.x =
		(((unsigned)buff[2] & 0x07) << 18) |
		((unsigned)buff[3] << 10) |
		((unsigned)buff[4] << 2) |
		((buff[5] & 0xc0) >> 6);
	v[2] = s.x =
		(((unsigned)buff[5] & 0x3f) << 15) |
		((unsigned)buff[6] << 7) |
		(buff[7] >> 1);
}

void RIFT_DK1::Decode(const char* buff){
	struct{
		int accel[3];
		int rotate[3];
	}sample[3];
	int mag[3];

	//NOTE:リトルエンディアン機で動かす前提
	const unsigned char numOfSamples(buff[1]);
	const unsigned short timestamp(*(unsigned short*)&buff[2]);
//	const short temp(*(short*)&buff[6]);

	const uint samples(numOfSamples > 2 ? 3 : numOfSamples);
	for(unsigned char i(0); i < samples; i++){
		DecodeSensor((unsigned char*)buff + 8 + 16 * i, sample[i].accel);
		DecodeSensor((unsigned char*)buff + 16 + 16 * i, sample[i].rotate);
	}
	//磁気センサのデータ取得(座標系が違うのでY-Zを交換)
	mag[0] = *(short*)&buff[56];
	mag[1] = *(short*)&buff[60];
	mag[2] = *(short*)&buff[58];

	static unsigned short prevTime;
	const unsigned short deltaT(timestamp - prevTime);
	prevTime = timestamp;

	const double qtime(1.0/1000.0);
	const double dt(qtime * deltaT / numOfSamples);

	// 各サンプル値で状況を更新
	for(unsigned char i(0); i < samples; i++){
		UpdateAngularVelocity(sample[i].rotate, dt);
		UpdateAccelaretion(sample[i].accel, dt);
	}

	// 磁界値取得
	UpdateMagneticField(mag);

	//温度取得
	UpdateTemperature(0.01 * *(short*)&buff[6]);

}

