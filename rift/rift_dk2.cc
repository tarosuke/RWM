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


RIFT_DK2::RIFT_DK2(int f) : RIFT(f, width, height){}
RIFT_DK2::~RIFT_DK2(){
}


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
	glTranslatef(-0.03, 0, 0);

	//記録と描画
	displayList.StartRecord(true);
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
	glTranslatef(0.03, 0, 0);
	displayList.Playback();

#if 0
	//Riftの歪み除去
	glGetError();
	glViewport(0, 0, width, height);
	assert(glGetError() == GL_NO_ERROR);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	assert(glGetError() == GL_NO_ERROR);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);
	assert(glGetError() == GL_NO_ERROR);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	//フラグメントシェーダによる歪み除去
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deDistorTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glUseProgram(deDistorShaderProgram);
	glUniform1i(glGetUniformLocation(deDistorShaderProgram, "buffer"), 0);
	glUniform1i(glGetUniformLocation(deDistorShaderProgram, "de_distor"), 1);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0, 0); glVertex3f(-1, -1, 0.5);
	glTexCoord2f(0, 1); glVertex3f(-1, 1, 0.5);
	glTexCoord2f(1, 0); glVertex3f(1, -1, 0.5);
	glTexCoord2f(1, 1); glVertex3f(1, 1, 0.5);
	glEnd();
	glUseProgram(0);
	assert(glGetError() == GL_NO_ERROR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, 0);
#endif
}

