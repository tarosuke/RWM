#ifndef _RIFT_
#define _RIFT_


class RIFT{
public:
	RIFT();
	~RIFT(){ ren = false; };
	void GetMatrix(double matrix[]);
private:
	volatile bool run;
	double rotate[16];
	static void SensorThread(void* initialData);
};


#endif
