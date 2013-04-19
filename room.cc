#include <stdio.h>

#include "room.h"


static FILE* scriptFile;






















void ROOM::Load(const char* scriptFilename){
	if(!(scriptFile = fopen(scriptFilename, "r"))){
		return;
	}
	printf("loading script:%s...", scriptFilename);
}
