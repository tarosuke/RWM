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
#include <settings/settings.h>


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

namespace{ const float MAXFLOAT(3.40282347e+38F); };
RIFT::RIFT() :
	fd(OpenDevice()),
	gravityAverageRatio(10),
	gravity(0.0, -G, 0.0),
	magAverageRatio(100),
	magFront(0.0, 0.0, 1.0),
	magMax(-MAXFLOAT, -MAXFLOAT, -MAXFLOAT),
	magMin(MAXFLOAT, MAXFLOAT, MAXFLOAT),
	magReady(false),
	magFinished(false),
	magneticField(0.0, 0.0, -0.01){
	if(fd < 0){
		printf("Could not locate Rift\n");
		printf("sutup udev: SUBSYSTEM==\"hidraw\",ATTRS{idVendor}==\"2833\",ATTRS{idProduct}==\"0001\",MODE=\"0666\"\n");
		return;
	}

	//過去の磁化情報があれば取得
	settings.Fetch("magFront", &magFront);
	settings.Fetch("magMax", &magMax);
	settings.Fetch("magMin", &magMin);

	//センサデータ取得開始
	pthread_create(&sensorThread, NULL, RIFT::_SensorThread, (void*)this);
}

RIFT::~RIFT(){
	if(0 <= fd){
		pthread_cancel(sensorThread);
		close(fd);
	}

	//磁化情報を保存
	settings.Store("magFront", &magFront);
	settings.Store("magMax", &magMax);
	settings.Store("magMin", &magMin);
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
	UpdateMagneticField(mag);

	//補正
	Correction();
}

void RIFT::Correction(){
	const QON direction(GetDirection());

	//重力による姿勢補正
	VQON down(0, 1, 0); //こうなっているはずの値(向きのみ)
	down.ReverseRotate(direction); //機体座標系にする

	//重力方向との差分で姿勢を補正
	QON differ(down, gravity);
	Rotate(differ);

	//磁気による姿勢補正
	if(magReady && !magFinished){
		//準備ができていて、かつまだ補正が完了していない
		VQON front(magFront); //正面(のはずの方位)
		VQON mag(magneticField);
		mag.Rotate(direction); //絶対基準にする

		//正面との差分で姿勢を補正
		QON magDiffer(mag, front);
		magDiffer.i = magDiffer.k = 0.0; //水平角以外をキャンセル
		magDiffer.Normalize();

		if(magAverageRatio < 1000 || magDiffer.w < 0.999999){
			RotateAzimuth(magDiffer);
		}else{
			//終了処理
			magFinished = true;
		}
	}
}


void RIFT::UpdateAngularVelocity(const int angles[3], double dt){
	QON delta(angles, 0.0001 * dt);
	Rotate(delta);
	const double norm(gravity.Abs());
	gravity.ReverseRotate(delta);
	gravity *= norm;
	magneticField.ReverseRotate(delta);
}

void RIFT::UpdateAccelaretion(const int axis[3], double dt){
	//キャリブレーションのファストスタート処理
	if(gravityAverageRatio < maxGravityAverageRatio){
		gravityAverageRatio++;
	}

	//加速度情報取得
	VQON acc(axis, 0.0001);

	//重力加速度分離
	VQON g(acc);
	const double ratio(1.0 / gravityAverageRatio);
	g *= ratio;
	gravity *= 1.0 - ratio;
	gravity += g;

	//重力を除去
	acc -= gravity;

	//位置や速度を求める
	if(dt <= 0.02){ //dtが異常に大きい時はデータが欠損しているので位置更新しない
		acc.Rotate(GetDirection()); //絶対座標系へ変換
		acc *= dt;
		velocity += acc;
		VQON v(velocity);
		v *= dt;
		position += v;
		MoveTo(position);

		//枠を出ていたら止める
		const float limit(0.3);
		if(position.i < - limit || limit < position.i){
			velocity.i = 0;
			position.i = position.i < 0 ? -limit : limit;
		}
		if(position.j < - limit || limit < position.j){
			velocity.j = 0;
			position.j = position.j < 0 ? -limit : limit;
		}
		if(position.k < - limit || limit < position.k){
			velocity.k = 0;
			position.k = position.k < 0 ? -limit : limit;
		}
		velocity *= 0.999;
		position *= 0.995;
	}
}

void RIFT::UpdateMagneticField(const int axis[3]){
	//キャリブレーションのファストスタート処理
	if(magAverageRatio < maxMagAverageRatio){
		magAverageRatio++;
	}

	//磁力情報取得
	VQON mag(axis, 1.0);

	//キャリブレーション
	magMax.Max(mag);
	magMin.Min(mag);
	VQON offset(magMax + magMin);
	offset *= 0.5;

	if(magReady){
		//磁化分を除去
		mag -= offset;
		mag.Normalize();

		//平均化処理
		mag *= 1.0 / magAverageRatio;
		magneticField *= 1.0 - 1.0 / magAverageRatio;
		magneticField += mag;
	}else{
		//キャリブレーション判定
		VQON d(magMax - magMin);
		if(7000 < abs(d.i) && 7000 < abs(d.j) && 7000 < abs(d.k)){
			magReady = true;
		}
	}
}

