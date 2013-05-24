#ifndef _RIFT_
#define _RIFT_

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
	Device* dev;
	Device device;
	volatile bool run;
	static void* _SensorThread(void* initialData);
	static const int VendorID = 0x2833;
	static const int ProductID = 0x0001;
	static const int keepAliveInterval = 1000;
	void SensorThread();
	void KeepAlive();
	void Sample();
	void Decode(const char* buff, int size);
};


#endif
