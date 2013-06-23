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
	bool gravityFirstCycle;
	bool magneticFirstCycle;
	static const int VendorID = 0x2833;
	static const int ProductID = 0x0001;
	static const int keepAliveInterval = 1000;
	static int OpenDevice();

	// SENSOR
	pthread_t sensorThread;
	QON direction; //方向の四元数(回転オペレータ)
	VQON velocity; //移動速度
	VQON position; //位置
	double gravity; //おそらく重力によるものと思われる平均重力加速度
	static void* _SensorThread(void* initialData);
	void SensorThread();
	static void DecodeSensor(const unsigned char* buff, int* const sample);
	void Decode(const char* buff);
	void UpdateAngularVelocity(const int angles[3], double dt);
	void UpdateAccelaretion(const int axis[3], double dt);
	void UpdateMagneticField(const int axis[3]);

	float temperature; // センサ表面温度[℃]
};


#endif
