#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <assert.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

#include "rift.h"
#include "../settings/settings.h"


FACTORY<VIEW> RIFT_DK2::factory(New);


VIEW* RIFT_DK2::New(){
	const int fd(OpenDeviceFile(ProductID));
	if(0 <= fd){
		//確保完了
		puts("Oculus Rift DK2 found.\n");
		return new RIFT_DK2(fd);
	}

	//なかった
	return 0;
}


RIFT_DK2::RIFT_DK2(int f) : RIFT(f, width, height){
	//歪み情報テクスチャを作る
	DISTORE_ELEMENT *body(
		(DISTORE_ELEMENT*)malloc(width * height * sizeof(DISTORE_ELEMENT)));
	assert(body);
	const unsigned hh(height / 2);
	for(unsigned v(0); v < hh; v++){
		for(unsigned u(0); u < width; u++){
			DISTORE_ELEMENT& b(body[v * width + u]);
			DISTORE_ELEMENT& d(body[(height - v - 1) * width + u]);

			P2 tc = {(float)u / hh - (float)0.5, (float)v / hh - (float)0.5};
			float dd(tc.u*tc.u + tc.v*tc.v);
			dd = 1.0 + 0.45 * dd + 0.5 * dd*dd + 0.9 * dd*dd*dd;
			tc.u *= dd;
			tc.v *= dd;

			if(-0.5 <= tc.v && tc.v < 0.5){
				b.u = tc.u;
				b.v = tc.v * 0.5;
				d.u = tc.u;
				d.v = -tc.v * 0.5;
			}else{
				b.u = d.u = b.v = d.v = -2.0;
			}
		}
	}

	//テクスチャ登録
	RegisterDeDistoreCoords(body);
	free(body);
}


RIFT_DK2::~RIFT_DK2(){}


void RIFT_DK2::PreDraw(){
	const float tf(GetTanFov() * nearDistance);
	const int hh(height / 2);
	const float ar((float)hh / width);

	//左目
	glViewport(0, 0, width, hh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-tf, tf, -ar * tf , ar * tf,
		nearDistance, farDistance);

	//Model-View行列初期化
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(90,0,0,1);
	glTranslatef(0.03, 0, 0);

	//記録と描画
	displayList.StartRecord(true);
}

void RIFT_DK2::PostDraw(){
	const float tf(GetTanFov() * nearDistance);
	const int hh(height / 2);
	const float ar((float)hh / width);

	displayList.EndRecord(); //記録完了

	//右目
	glViewport(0, hh, width, hh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-tf, tf, -ar * tf , ar * tf,
		nearDistance, farDistance);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(90,0,0,1);
	glTranslatef(-0.03, 0, 0);
	displayList.Playback();

	//収差補正
	DeDistore();
}

