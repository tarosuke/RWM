
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


FACTORY<VIEW> RIFT_DK1::factory(New);


VIEW* RIFT_DK1::New(){
	const int fd(OpenDeviceFile(ProductID));
	if(0 <= fd){
		//確保完了
		puts("Oculus Rift DK1 found.\n");
		return new RIFT_DK1(fd);
	}

	//なかった
	return 0;
}


RIFT_DK1::RIFT_DK1(int f) : RIFT(f, width, height){
	//歪み情報テクスチャを作る
	SetupDeDistoreTexture();
}
RIFT_DK1::~RIFT_DK1(){}

void RIFT_DK1::PreDraw(){
	const float tf(GetTanFov() * nearDistance);
	const int hw(width / 2);
	const float ar(width / height);

	//左目
	glViewport(0, 0, hw, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum((-ar - inset) * tf , (ar - inset) * tf,
		  -tf, tf, nearDistance, farDistance);

	//Model-View行列初期化
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.03, 0, 0);

	//記録と描画
	displayList.StartRecord(true);
	glScalef(1.0, 0.5, 1.0);
}
void RIFT_DK1::PostDraw(){
	const float tf(GetTanFov() * nearDistance);
	const int hw(width / 2);
	const float ar(width / height);

	displayList.EndRecord(); //記録完了

	//右目
	glViewport(0, 0, hw, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum((-ar - inset) * tf , (ar - inset) * tf,
		  -tf, tf, nearDistance, farDistance);

	//Model-View行列初期化
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.03, 0, 0);
	displayList.Playback();

	//収差補正
	DeDistore();
}



void RIFT_DK1::SetupDeDistoreTexture(){
	DISTORE_ELEMENT *body(
		(DISTORE_ELEMENT*)malloc(width * height * sizeof(DISTORE_ELEMENT)));
	assert(body);
	const float rightSide((float)(width - 1)/width);
	const float halfRight((float)(width/2 - 2)/width);
	for(unsigned v(0); v < height; v++){
		for(unsigned u(0); u < width / 2; u++){
			DISTORE_ELEMENT& b(body[v*width + u]);
			DISTORE_ELEMENT& d(body[v*width + width-u-1]);
			P2 tc(GetTrueCoord(u, v));
			tc.u /= width;
			tc.v /= height;
			if(tc.u < 0.0 || halfRight <= tc.u ||
				tc.v < 0.0 || 1.0 <= tc.v){
				// 範囲外
				b.u = d.u = b.v = d.v = -2.0;
			}else{
				// 座標を書き込む
				b.u = tc.u;
				d.u = rightSide - tc.u;
				b.v =
				d.v = tc.v;
			}
		}
	}

	//テクスチャ登録
	RegisterDeDistoreCoords(body);
	free(body);
}


