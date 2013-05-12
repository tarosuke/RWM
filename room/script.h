#ifndef _SCRIPT_
#define _SCRIPT_


class SCRIPT{
public:
	SCRIPT(const char* pathFromHome);
	~SCRIPT();
	void Rewind();
	const char* getNextToken();
private:
	struct FILE* flie;
};

#endif
