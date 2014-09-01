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
	SetupDeDistoreTexture();
}
RIFT_DK2::~RIFT_DK2(){}


void RIFT_DK2::PreDraw(){
	const float tf(GetTanFov() * nearDistance);
	const int hh(height / 2);
	const float ar((float)height / width);

	//左目
	glViewport(0, 0, width, hh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-tf, tf, (-ar - inset) * tf , (ar - inset) * tf,
		nearDistance, farDistance);

	//Model-View行列初期化
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(90,0,0,1);
	glTranslatef(0.03, 0, 0);

	//記録と描画
	displayList.StartRecord(true);
	glScalef(1.0, 0.5, 1.0);
}

void RIFT_DK2::PostDraw(){
	const float tf(GetTanFov() * nearDistance);
	const int hh(height / 2);
	const float ar((float)height / width);

	displayList.EndRecord(); //記録完了

	//右目
	glViewport(0, hh, width, hh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-tf, tf, (-ar + inset) * tf , (ar + inset) * tf,
		nearDistance, farDistance);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(90,0,0,1);
	glTranslatef(-0.03, 0, 0);
	displayList.Playback();

	//収差補正
	DeDistore();
}



void RIFT_DK2::SetupDeDistoreTexture(){
	struct DISTORE_ELEMENT{
		float u;
		float v;
	}__attribute__((packed)) *body(
		(DISTORE_ELEMENT*)malloc(width * height * sizeof(DISTORE_ELEMENT)));
	assert(body);
	const unsigned hh(height / 2);
	const float bs((float)(height - 1)/height);
	for(unsigned v(0); v < hh; v++){
		for(unsigned u(0); u < width; u++){
			DISTORE_ELEMENT& b(body[v * width + u]);
			DISTORE_ELEMENT& d(body[(height - v - 1) * width + u]);
			P2 tc = {(float)u / width, (float)v / hh};
			static const P2 cc = { 0.5, 0.5 + inset };
			P2 fe = { tc.u - cc.u, tc.v - cc.v };
			float dd(fe.u*fe.u*4 + fe.v*fe.v);
			dd = 1.0 + 0.625 * dd + 0.5 * dd*dd + 0.125 * dd*dd*dd;
			fe.u *= dd;
			fe.v *= dd;
			tc.u = fe.u + cc.u;
			tc.v = fe.v + cc.v;

			if(0 <= tc.u && tc.u < 1.0 && 0 <= tc.v && tc.v < 1.0){
				b.u = tc.u;
				b.v = tc.v * 0.5;
				d.u = tc.u;
				d.v = bs - tc.v * 0.5;
			}else{
				b.u = d.u = b.v = d.v = -2.0;
			}
		}
	}

	glGenTextures(1, &deDistorTexture);
	glBindTexture(GL_TEXTURE_2D, deDistorTexture);
	glTexParameteri(
		GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(
		GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	assert(glGetError() == GL_NO_ERROR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RG16F,
	      width, height, 0, GL_RG, GL_FLOAT, body);
	free(body);
	assert(glGetError() == GL_NO_ERROR);

	glBindTexture(GL_TEXTURE_2D, 0);
}



