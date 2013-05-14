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
	static void GetMatrix(double matrix[]);
private:
	static Device* dev;
	static Device device;
	static volatile bool run;
	static void* SensorThread(void* initialData);
	static const int VendorID = 0x2833;
	static const int ProductID = 0x0001;
};


#endif
