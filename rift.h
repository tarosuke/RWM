#ifndef _RIFT_
#define _RIFT_


class RIFT{
public:
	RIFT();
	~RIFT(){ run = false; };
	void GetMatrix(double matrix[]);
private:
	static volatile bool run;
	double rotate[16];
	static int SensorThread(void* initialData);
};


#endif
