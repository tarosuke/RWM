#ifndef _RIFT_
#define _RIFT_


class RIFT{
public:
	RIFT();
	~RIFT(){ run = false; };
	void GetMatrix(double matrix[]);
private:
	static volatile bool run;
	static double rotation[];
	static void* SensorThread(void* initialData);
};


#endif
