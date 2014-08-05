#pragma once

#include <pthread.h>

#include "../view.h"
#include "../gl/displayList.h"



class RIFT : public VIEW{
	RIFT();
	RIFT(const RIFT&);
	void operator=(const RIFT&);
public:
protected:
	RIFT(unsigned w, unsigned h);

	void UpdateAngularVelocity(const int[3], double);
	void UpdateAccelaretion(const int[3], double);
	void UpdateMagneticField(const int[3]);
	void UpdateTemperature(float){};

private:
	const POSE& Pose() const{ return pose; };
	POSE pose;

	/////センサ関連

	//補正のファストスタート処理
	static const int initialAverageRatio = 3;
	static const int maxAverageRatio = 10000;
	float averageRatio;

	//加速度センサ(絶対座標系)
	VECTOR<3> velocity; //移動速度
	VECTOR<3> gravity; //平均加速度(機体座標系)

	//磁気センサ
	VECTOR<3> magMax; //磁気センサの最大値
	VECTOR<3> magMin; //磁気センサの最小値
	VECTOR<3> magFront; //正面にする方位
	bool magReady; //磁化情報取得完了
	VECTOR<3> magneticField; //磁気の向き(機体座標系)

	// 温度センサ[℃]
	float temperature;

	/////VIEW関連
	int deDistorShaderProgram;
	static const char* vertexShaderSource;
	static const char* fragmentShaderSource;
	struct P2{
		float u;
		float v;
	};
	P2 GetTrueCoord(float u, float v);
	static float D(float l);
	unsigned framebufferTexture;
	unsigned deDistorTexture;

	//画面の大きさなど
	const unsigned width;
	const unsigned height;
	static const float inset = 0.1453 + 0.04;

	//描画前＆描画後
	void PreDraw(){}; //描画領域、東映行列の設定、displayList記録開始
	void PostDraw(){}; //右目用設定、displayList再生による再描画、歪み除去

	//再描画用
	GL::DisplayList displayList;

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

	// HID
	const int fd;
	static const int VendorID = 0x2833;
	static const int ProductID = 0x0001;
	static const long keepaliveInterval = 1000;
	void Keepalive();

	// SENSOR
	pthread_t sensorThread;

	// 受信処理
	static void* _SensorThread(void* initialData);
	void SensorThread();

	//受信データのデコード
	void DecodeSensor(const unsigned char*, int[3]);
	void Decode(const char*);
};
