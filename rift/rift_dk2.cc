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
	//Riftのセンサを準備
	for(int i(0); i < 99; i++){
		char name[32];
		snprintf(name, 32, "/dev/hidraw%d", i);
		const int fd(open(name, O_RDWR | O_NONBLOCK));
		if(fd < 0){
			//開けなかった
			continue;
		}

		struct hidraw_devinfo info;
		if(ioctl(fd, HIDIOCGRAWINFO, &info) < 0){
			//ioctlできない=riftではない
			close(fd);
			continue;
		}
		if(VendorID != info.vendor || ProductID != info.product){
			//riftではない
			close(fd);
			continue;
		}

		if(flock(fd, LOCK_EX | LOCK_NB) < 0){
			//使用中
			close(fd);
			continue;
		}

		//確保完了
		printf("Oculus Rift DK2:%s.\n", name);
		return new RIFT_DK2(fd);
	}

	//なかった
	return 0;
}


RIFT_DK2::RIFT_DK2(int f) :
	RIFT(f, width, height){

#if 0
	//過去の磁化情報があれば取得
	settings.Fetch("magMax", &magMax);
	settings.Fetch("magMin", &magMin);
	settings.Fetch("magFront", &magFront);
#endif

	//スケジューリングポリシーを設定
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

	//デバイスを閉じないカーネルバグ対策で最初に一発Keepalive
	Keepalive();

	//センサデータ取得開始
	pthread_create(&sensorThread, &attr, _SensorThread, (void*)this);
}


RIFT_DK2::~RIFT_DK2(){
	pthread_cancel(sensorThread);
	pthread_join(sensorThread, 0);
	close(fd);

	//磁化情報を保存
#if 0
	settings.Store("magMax", &magMax);
	settings.Store("magMin", &magMin);
	settings.Store("magFront", &magFront);
#endif
}

void RIFT_DK2::SensorThread(){
	//優先度設定
	pthread_setschedprio(
		sensorThread,
		sched_get_priority_max(SCHED_FIFO));

	//Riftからのデータ待ち、処理
	fd_set readset;

	FD_ZERO(&readset);
	FD_SET(fd, &readset);

	for(;; pthread_testcancel()){
		int result(select(
			fd + 1, &readset, NULL, NULL, NULL));

		if(result && FD_ISSET( fd, &readset )){
			char buff[256];
			const int rb(read(fd, buff, 256));
			if(62 == rb){
				Decode(buff);
			}else{
				printf("%5d bytes dropped.\n", rb);
			}
		}

		//KeepAliveを送信
		Keepalive();
	}
}
void* RIFT_DK2::_SensorThread(void* initialData){
	//オブジェクトを設定して監視開始
	(*(RIFT_DK2*)initialData).SensorThread();
	return 0;
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

