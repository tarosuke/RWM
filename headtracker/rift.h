/**************************************************** Oculus Rift handler:rift
 *
 */
#pragma once

#include <pthread.h>

#include "headtracker.h"


class RIFT : public HEADTRACKER{
public:
	typedef COMPLEX<4> QON;
	typedef VECTOR<3> VQON;
	RIFT();
	~RIFT();
	bool IsEnable() const { return 0 <= fd; };
private:
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
	const static int maxGravityAverageRatio = 100000;
	int gravityAverageRatio;
	VQON velocity; //移動速度
	VQON position; //位置(変位)
	VQON gravity; //平均加速度(機体座標系)

	//磁気センサ
	const static int maxMagAverageRatio = 1000;
	int magAverageRatio;
	VQON magMax; //磁気センサの最大値
	VQON magMin; //磁気センサの最小値
	VQON magFront; //正面にする方位
	bool magReady; //磁化情報取得完了
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
	static const int correctionGainSeed = 64;
	double GetCorrectionGain(const COMPLEX<4>& diff) const;

	void Rotate(const COMPLEX<4>&);

	void Keepalive();
};


