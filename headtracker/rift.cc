/**************************************************** Oculus Rift handler:rift
 *
 */
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
		printf("Oculus Rift:%s.\n", name);
		return fd;
	}
	return -1;
}

namespace{ const float MAXFLOAT(3.40282347e+38F); };
RIFT::RIFT() :
	fd(OpenDevice()),
	gravityAverageRatio(10),
	gravity((const double[]){ 0.0, 0-G, 0.0 }),
	magAverageRatio(100),
	magMax((const double[]){ 0-MAXFLOAT, 0-MAXFLOAT, 0-MAXFLOAT }),
	magMin((const double[]){ MAXFLOAT, MAXFLOAT, MAXFLOAT }),
	magFront((const double[]){ 0, 0, 1 }),
	magReady(false),
	magneticField((const double[3]){ 0.0, 0.0, 0.01 }){
	if(fd < 0){
		printf("Could not locate Rift\nsetup /etc/udev/rules.d/50-oculusRift.rules as SUBSYSTEM==\"hidraw\",ATTRS{idVendor}==\"2833\",ATTRS{idProduct}==\"0001\",MODE=\"0666\"\n");
		return;
	}

	//過去の磁化情報があれば取得
	settings.Fetch("magMax", &magMax);
	settings.Fetch("magMin", &magMin);
	settings.Fetch("magFront", &magFront);

	//スケジューリングポリシーを設定
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

	//デバイスを閉じないカーネルバグ対策のため最初に一発Keepalive
	Keepalive();

	//センサデータ取得開始
	pthread_create(&sensorThread, &attr, RIFT::_SensorThread, (void*)this);
}

RIFT::~RIFT(){
	if(0 <= fd){
		pthread_cancel(sensorThread);
		pthread_join(sensorThread, 0);
		close(fd);
	}

	//磁化情報を保存
	settings.Store("magMax", &magMax);
	settings.Store("magMin", &magMin);
//	settings.Store("magFront", &magFront);
}

void RIFT::Keepalive(){
	char buff[5];
	buff[0] = 8;
	buff[1] = buff[2] = 0; //command ID
	buff[3] = keepAliveInterval & 0xFF;
	buff[4] = keepAliveInterval >> 8;
	ioctl(fd, HIDIOCSFEATURE(5), buff);
}

void RIFT::SensorThread(){
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
void* RIFT::_SensorThread(void* initialData){
	//オブジェクトを設定して監視開始
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
	const double dt(qtime * deltaT / numOfSamples);

	// 各サンプル値で状況を更新
	for(unsigned char i(0); i < samples; i++){
		UpdateAngularVelocity(sample[i].rotate, dt);
		UpdateAccelaretion(sample[i].accel, dt);
	}

	// 磁界値取得
	UpdateMagneticField(mag);

	//温度取得
	temperature = 0.01 * temp;

	//補正
	Correction();
}

void RIFT::Correction(){
	const QON direction(GetDirection());

	//重力による姿勢補正
	VQON down((const double[]){ 0, 1, 0 }); //こうなっているはずの値(向きのみ)
	down.ReverseRotate(direction); //機体座標系にする

	//重力方向との差分で姿勢を補正
	QON differ(down, gravity);
	differ *= GetCorrectionGain(differ);
	Rotate(differ);

	//磁気による姿勢補正
	if(magReady && magAverageRatio < maxMagAverageRatio){
		//準備ができていて、かつまだ補正が完了していない
		VQON mag(magneticField);
		mag.Rotate(direction); //絶対基準にする

		//正面との差分で姿勢を補正
		QON magDiffer(magFront, mag);
		magDiffer.FilterAxis(2); //水平角以外をキャンセル
		magDiffer *= GetCorrectionGain(magDiffer);
		RotateAzimuth(magDiffer);
		gravity.ReverseRotate(magDiffer);
	}
}


void RIFT::UpdateAngularVelocity(const int angles[3], double dt){
	QON delta(angles, 0.0001 * dt);
	Rotate(delta);
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
		double* p(position);
		double* ve(velocity);
		for(unsigned n(0); n < 3; ++n){
			if(p[n] < - limit || limit < p[n]){
				ve[n] = 0;
				p[n] = p[n] < 0 ? -limit : limit;
			}
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
	VQON mag(axis);

	//キャリブレーション
	magMax.Max(mag);
	magMin.Min(mag);
	const VECTOR<3> deGain(magMax - magMin);
	const double* const d(deGain);

	if(magReady){
		//磁化分を除去
		VQON offset(magMax + magMin);
		offset *= 0.5;
		mag -= offset;

		//各軸ゲイン調整
		double* const g(mag);
		g[0] /= d[0];
		g[1] /= d[1];
		g[2] /= d[2];
		mag.Normalize();

		//平均化処理
		mag *= 1.0 / magAverageRatio;
		magneticField *= 1.0 - 1.0 / magAverageRatio;
		magneticField += mag;
	}else{
		//キャリブレーション判定
		if(7000 < abs(d[0]) && 7000 < abs(d[1]) && 7000 < abs(d[2])){
			magReady = true;
			magAverageRatio = 100; //キャリブレーションのやり直し
			puts("magnetic azimuth correction READY.");
		}
	}
}


double RIFT::GetCorrectionGain(const COMPLEX<4>& diff) const{
	double gain(diff.Norm());
	gain *= gain * correctionGainSeed;
	return gain / (correctionGainSeed * correctionGainSeed);
}

void RIFT::Rotate(const COMPLEX<4>& r){
	HEADTRACKER::Rotate(r);
	const double norm(gravity.Abs());
	gravity.ReverseRotate(r);
	gravity *= norm;
	magneticField.ReverseRotate(r);
}


