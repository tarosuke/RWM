#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "room.h"
#include "wall.h"
#include "primitive.h"
#include "texture.h"


static FILE* scriptFile;
ROOM* ROOM::userIn(0);
const char* ROOM::rcPath = ".rwm/room.cf";




ROOM::ROOM(){
}


ROOM::~ROOM(){
}




void ROOM::Draw(int remain){
	//部屋の明るさ
	glLightfv(GL_LIGHT0, GL_AMBIENT, brightness.raw);

	//TODO:壁や床に開ける穴(描画する代わりにステンシルバッファを-1)

	//壁を一気に配置(ステンシルっバッファ <= remainの時だけ)
	glBegin(GL_QUAD_STRIP);
	for(int c(0); c < numOfWalls; c++){
// 		if(walls.texture.id){
// 			glBindTexture(GL_TEXTURE_2D, 0);
// 		}
		glMaterialfv(GL_FRONT_AND_BACK,
			GL_AMBIENT_AND_DIFFUSE,
			walls[c].texture.color.raw);
		glNormal3fv(walls[c].normal);
// 		glVertex3fv(vp[v]);
		glVertex3f( walls[c].x, ceilHeight, walls[c].z);
// 		glVertex3fv(vp[v]);
		glVertex3f( walls[c].x, floorHeight, walls[c].z);
	}
	glVertex3f( walls[0].x, ceilHeight, walls[0].z);
	glVertex3f( walls[0].x, floorHeight, walls[0].z);
	glEnd();

	//床

	//天井

	//TODO:固定オブジェクト

	//TODO:可動オブジェクト

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
		static TESTROOM testRoom;
	}
}

void ROOM::Unload(){
}
