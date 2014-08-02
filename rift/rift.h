#pragma once

#include <pthread.h>

#include "../view.h"



class RIFT : public VIEW{
	RIFT();
	RIFT(const RIFT&);
	void operator=(const RIFT&);
public:
protected:
	RIFT(unsigned w, unsigned h) : VIEW(w, h){};
private:
};


class RIFT_DK1 : public RIFT{
	RIFT_DK1();
	RIFT_DK1(const RIFT_DK1&);
	void operator=(const RIFT_DK1&);
public:
	static VIEW* New();
private:
	static FACTORY<VIEW> factory;
	static const unsigned width = 1280;
	static const unsigned height = 800;


	//VIEWとしてのインターフェイス
	RIFT_DK1(int);
	~RIFT_DK1();
	void PreDraw(){};
	void PostDraw(){};
	const POSE& Pose() const{ return pose; };
	POSE pose;


	// HID
	const int fd;
	static const int VendorID = 0x2833;
	static const int ProductID = 0x0001;
	static const long keepaliveInterval = 1000;
	static const char keepaliveCommand[];
	void Keepalive();

	// SENSOR
	pthread_t sensorThread;

	//ファストスタート処理
	static const int initialAverageRatio = 3;
	static const int maxAverageRatio = 10000;
	float averageRatio;

	//加速度センサ(絶対座標系)
	VECTOR<3> velocity; //移動速度
	VECTOR<3> position; //位置(変位)
	VECTOR<3> gravity; //平均加速度(機体座標系)

	//磁気センサ
	VECTOR<3> magMax; //磁気センサの最大値
	VECTOR<3> magMin; //磁気センサの最小値
	VECTOR<3> magFront; //正面にする方位
	bool magReady; //磁化情報取得完了
	VECTOR<3> magneticField; //磁気の向き(機体座標系)

	// 温度センサ[℃]
	float temperature;

	// 受信処理
	static void* _SensorThread(void* initialData);
	void SensorThread();

	//受信データのデコード
	void Decode(const char*);
};
