
#pragma once


class SNOW{
public:
	static void DrawAll();
	SNOW();
private:
	static const unsigned numOfParticles = 1000;
	struct P{
		float x;
		float y;
		float z;
	}__attribute__((packed));
	static P positions[];
	static SNOW particles[];
	static float R();

	P& p;
	void Run();
};

