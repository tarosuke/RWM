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
	gravity(G),
	magMax(-FP_INFINITE, -FP_INFINITE, -FP_INFINITE),
	magMin(FP_INFINITE, FP_INFINITE, FP_INFINITE),
	magReadyX(false),
	magReadyY(false),
	magReadyZ(false),
	magneticField(0.0, 0.0, -0.01){
	if(fd < 0){
		printf("Could not locate Rift\n");
		printf("sutup udev: SUBSYSTEM==\"hidraw\",ATTRS{idVendor}==\"2833\",ATTRS{idProduct}==\"0001\",MODE=\"0666\"\n");
		return;
	}

	//センサデータ取得開始
	pthread_create(&sensorThread, NULL, RIFT::_SensorThread, (void*)this);
}

RIFT::~RIFT(){
	if(0 <= fd){
		pthread_cancel(sensorThread);
		close(fd);
	}
}


void RIFT::SensorThread(){
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
	//磁気センサのデータ取得(座標系が違うのでY-Zを交換)
	mag[0] = *(short*)&buff[56];
	mag[1] = *(short*)&buff[60];
	mag[2] = *(short*)&buff[58];

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

	// 磁界値取得
#if 0
	UpdateMagneticField(mag);
#endif

	//補正
	Correction();
}

void RIFT::Correction(){
	const QON direction(GetDirection());

	//重力による姿勢補正
	const double g(accel.Length());
	const double d((gravity - g) * 50);
	const double gravityUnreliability(d*d);
	VQON acc(accel);
	acc.Normalize();

	//重力加速度の実測値を更新
	gravity *= 0.99;
	gravity += g * 0.01;

	//正しいはずの方向
	VQON down(0, -1, 0); //こうなっているはずの値
	down.ReverseRotate(direction);

	//重力方向との差分で姿勢を補正
	QON differ(acc, down);
	const double in(acc.In(down) * 10);
	const double nearRatio(in * in * in * in / 100);
	differ *= 0.005 / (0.5 + gravityUnreliability + nearRatio);
	Rotate(differ);

	//磁気による姿勢補正
#if 0
	VQON north(-1.0, 0.0, 0.0); //北(のはずの方向)
	north.ReverseRotate(direction); //機体基準に変換

	//北との差分で姿勢を補正
	QON magDiffer(magneticField, north);
// 	magDiffer *= direction; //絶対方位に変換
// 	magDiffer.i = magDiffer.k = 0.0; //水平角以外をキャンセル
// 	magDiffer *= -direction; //機体基準に変換

// 	magDiffer.Normalize();
	const double mdr(magDiffer.Length());
	magDiffer *= 0.001 * mdr * mdr;
	Rotate(magDiffer);
#endif
}


void RIFT::UpdateAngularVelocity(const int angles[3], double dt){
	QON delta(angles, 0.0001 * dt);
	Rotate(delta);
	magneticField.ReverseRotate(delta);
}

void RIFT::UpdateAccelaretion(const int axis[3], double dt){
	VQON acc(axis, 0.0001);
	accel = acc;

	//位置や速度を求める
	if(dt <= 0.02){ //dtが異常に大きい時は位置更新しない
		acc.Rotate(GetDirection());
		acc.j -= gravity;
		acc *= dt;
		velocity += acc;
		velocity *= 0.99;
		VQON v(velocity);
		v *= dt;
		position += v;
		position *= 0.99;
	}
}

void RIFT::UpdateMagneticField(const int axis[3]){
	VQON mag(axis, 1.0);

	//キャリブレーション
	magMax.Max(mag);
	magMin.Min(mag);
	VQON offset(magMax + magMin);
	offset *= 0.5;

	//磁化分を除去
	mag -= offset;
	mag.Normalize();

	//平均化処理
	mag *= 1.0 / magAverageRatio;
	magneticField *= 1.0 - 1.0 / magAverageRatio;
	magneticField += mag;
}

