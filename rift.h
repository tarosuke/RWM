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
	const int fd;
	Device* dev;
	Device device;
	volatile bool run;
	static const int VendorID = 0x2833;
	static const int ProductID = 0x0001;
	static const int keepAliveInterval = 1000;
	static int OpenDevice();
	static void* _SensorThread(void* initialData);
	void SensorThread();
	struct REPORT{
		unsigned char samples;
		unsigned short timestamp;
		unsigned short lastCommandID;
		short temperature;
		struct S{
			struct V3{
				int x;
				int y;
				int z;
			}accel, rotate;
		}sample[3];
		struct{
			short x;
			short y;
			short z;
		}mag;
	}report;
	static void DecodeSensor(const char* buff, REPORT::S::V3& sample);
	void Decode(const char* buff);
};


#endif
