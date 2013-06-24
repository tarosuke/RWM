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
	static float far(-100.0);
	static float dir(0.2);

	if(!tex){
		tex = new TEXTURE("AK2509.ppm");
	}

const float vp[][2]={
	{ -0.5, -1 },
	{ 0.5, -1 },
	{ -0.5, 1 },
	{ 0.5, 1 },
};
const float tp[][2]={
	{ -0.5, 2 },
	{ 0.5, 2 },
	{ -0.5, 0 },
	{ 0.5, 0 },
};

	glPushMatrix();
// 	far = -1;
	glTranslatef(0, 0, far);
	(*tex).Bind();
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0, 0, 1);
	for(int v(0); v < 4; v++){
		glTexCoord2fv(tp[v]);
		glVertex2fv(vp[v]);
	}
	glEnd();

	glPopMatrix();
	glDisable(GL_LIGHT0);

	glBindTexture(GL_TEXTURE_2D, 0);
	if((far <= -200.0 && dir < 0.0) || (100.0 <= far && 0.0 < dir)){
		dir = -dir;
	}
	far += dir;
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
