#include <stdio.h>
#include <stdlib.h>

#include "room.h"
#include "wall.h"
#include "primitive.h"


static FILE* scriptFile;
ROOM* ROOM::userIn(0);
Q<ROOM> ROOM::rooms;
const char* ROOM::rcPath = ".rwm/room.cf";

float ROOM::far(-100.0);
float ROOM::dir(0.2);



ROOM::ROOM() : Q<ROOM>::NODE(rooms){
}


ROOM::~ROOM(){
}















void ROOM::Load(bool test){
	if(!test){
		char path[256];
		snprintf(path, 256, "%s/%s", getenv("HOME"), rcPath);
		if(!(scriptFile = fopen(path, "r"))){
			printf("failed to load :%s\n", path);
			return;
		}
		printf("loading script:%s...", path);
		puts("OK.");
	}else{
		userIn = new ROOM();
	}
}

void ROOM::Unload(){
	rooms.Unload();
}
