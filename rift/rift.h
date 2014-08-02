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
	void PreDraw(){};
	void PostDraw(){};
	const POSE& Pose() const{ return pose; };












	// HID
	const int fd;
	static const int VendorID = 0x2833;
	static const int ProductID = 0x0001;
	static const long keepAliveInterval = 1000;

	// SENSOR
	static const double G = 9.80665;
	pthread_t sensorThread;

	POSE pose;



};
