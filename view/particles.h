
#pragma once


class PARTICLES{
public:
	static void DrawAll();
	PARTICLES();
private:
	static const unsigned numOfParticles = 1000;
	struct P{
		float x;
		float y;
		float z;
	}__attribute__((packed));
	static P positions[];
	static PARTICLES particles[];
	static float R();

	P& p;
	void Run();
};

