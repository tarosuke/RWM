#ifndef _RIFT_
#define _RIFT_

#include <pthread.h>

#include "qon.h"


class RIFT{
public:
	RIFT();
	~RIFT();
	void GetView();
	bool IsEnable(){ return 0 <= fd; };
private:
	// HID
	const int fd;
	static const int VendorID = 0x2833;
	static const int ProductID = 0x0001;
	static const int keepAliveInterval = 1000;
	static int OpenDevice();

	// SENSOR
	static const double G = 9.80665;
	pthread_t sensorThread;
	QON direction; //方向の四元数(回転オペレータ)

	VQON accel; //加速度
	VQON velocity; //移動速度
	VQON position; //位置
	double gravity; //おそらく重力によるものと思われる平均重力加速度

	static const double magMaxValue = 0x7fffffff;
	VQON magMax; //磁気センサの最大値
	VQON magMin; //磁気センサの最小値
	bool magReadyX;
	bool magReadyY;
	bool magReadyZ;
	VQON magneticField; //磁気の向き

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
