#ifndef _RIFT_
#define _RIFT_

#include <pthread.h>

#include "headtracker.h"


class RIFT : public HEADTRACKER{
public:
	RIFT();
	~RIFT();
	bool IsEnable() const { return 0 <= fd; };
private:
	virtual const STATE& GetState() const;

	//インターフェイス用データ
	mutable STATE state;
	struct PACK{
		STATE* avail;
		STATE states[2];
	};
	PACK& pack;

	//最新情報
	unsigned serial;
	STATE status;
	void Rotate(const QON& r){ status.direction *= r; };
	void RotateAzimuth(const QON& r){ status.azimuth *= r; };
	void MoveTo(const VQON& d){ status.place = d; };
	QON GetDirection(){
		QON direction(status.azimuth);
		direction *= status.direction;
		return direction;
	};
	void Update(){
		STATE& t(pack.states[++serial & 1]);
		t = status;
		pack.avail = &t;
	};

	// HID
	const int fd;
	static const int VendorID = 0x2833;
	static const int ProductID = 0x0001;
	static const long keepAliveInterval = 1000;
	static int OpenDevice();

	// SENSOR
	static const double G = 9.80665;
	pthread_t sensorThread;

	//加速度センサ(絶対座標系)
	const static int maxGravityAverageRatio = 1000000;
	int gravityAverageRatio;
	VQON velocity; //移動速度
	VQON position; //位置(変位)
	VQON gravity; //平均加速度(機体座標系)

	//磁気センサ
	const static int maxMagAverageRatio = 1000000;
	int magAverageRatio;
	VQON magFront; //正面の方位
	VQON magMax; //磁気センサの最大値
	VQON magMin; //磁気センサの最小値
	bool magReady; //磁化情報取得完了
	bool magFinished; //方位補正完了
	VQON magneticField; //磁気の向き(機体座標系)

	float temperature; // センサ表面温度[℃]

	static void* _SensorThread(void* initialData);
	void SensorThread();
	static void DecodeSensor(const unsigned char* buff, int* const sample);
	void Decode(const char* buff);
	void UpdateAngularVelocity(const int angles[3], double dt);
	void UpdateAccelaretion(const int axis[3], double dt);
	void UpdateMagneticField(const int axis[3]);

	void Correction();
};


#endif
