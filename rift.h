#ifndef _RIFT_
#define _RIFT_

#include "qon.h"

extern "C"{
	#include <OVR.h>
	#include <OVR_HID.h>
}


class RIFT{
public:
	RIFT();
	~RIFT(){ run = false; };
	void GetMatrix(double matrix[]);
	bool IsEnable(){ return !!dev; };
private:
	// HID
	const int fd;
	Device* dev;
	Device device;
	volatile bool run;
	static const int VendorID = 0x2833;
	static const int ProductID = 0x0001;
	static const int keepAliveInterval = 1000;
	static int OpenDevice();

	// SENSOR
	QON direction; //方向の四元数(回転オペレータ)
	double velocity[3]; //移動速度
	double position[3]; //位置
	double north[3]; //北を向いているハズのベクトル
	double gravity[3]; //向きを整えた平均加速度ベクトル
	static void* _SensorThread(void* initialData);
	void SensorThread();
	static void DecodeSensor(const unsigned char* buff, int* const sample);
	void Decode(const char* buff);
	void UpdateAngularVelocity(const int angles[3], double dt);
	void UpdateAccelaretion(const int axis[3], double dt);
	void UpdateMagneticField(const int axis[3], double dt);

	float temperature; // センサ表面温度[℃]
};


#endif
