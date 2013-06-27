#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "room.h"
#include "wall.h"
#include "primitive.h"
#include "texture.h"


static FILE* scriptFile;
ROOM* ROOM::userIn(0);
Q<ROOM> ROOM::rooms;
const char* ROOM::rcPath = ".rwm/room.cf";




ROOM::ROOM() : Q<ROOM>::NODE(rooms){
}


ROOM::~ROOM(){
}

static TEXTURE* tex(0);

void ROOM::Draw(int remain){
	if(!tex){
		tex = new TEXTURE("AK2502.ppm");
	}
const float h(1.0);
const float vp[][3]={
	{ -1, -h, 2 },
	{ 3, -h, 2 },
	{ -1, -h, -4 },
	{ 3, -h, -4 }
};
const float cSize(0.3);
const float tp[][2]={
	{ 0, 0 },
	{ 3/cSize, 0 },
	{ 0, 6/cSize },
	{ 3/cSize, 6/cSize },
};

	glPushMatrix();
	(*tex).Bind();
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0, 1, 0);
	for(int v(0); v < 4; v++){
		glTexCoord2fv(tp[v]);
		glVertex3fv(vp[v]);
	}
	glEnd();

	glPopMatrix();
	glDisable(GL_LIGHT0);

	glBindTexture(GL_TEXTURE_2D, 0);
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
