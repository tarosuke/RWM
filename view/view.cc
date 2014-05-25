/************************************************************ view(rootwindow)
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#include "view.h"
#include <window.h>
#include <server.h>
#include <toolbox/complex/complex.h>

#include "snow.h"



bool VIEW::keep(true);


VIEW::VIEW(HEADTRACKER& head_) : head(head_){
	//画面サイズを取得
	width = x.Width();
	height = x.Height();

	//画面の実位置を仮定(スクリプトで上書き予定)
	realWidth = defaultDotPitch * width;
	realHeight = defaultDotPitch * height;
	realDistance = defaultDisplayDistance;
}

VIEW::~VIEW(){
}


void VIEW::Run(){
	while(keep){
		//Xのイベント処理
		x.EventHandler();
		xvfb.EventHandler();

		//バッファのクリア
		glClear(GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);

		//基本設定
		glEnable(GL_POLYGON_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);



		//(投影行列初期化も含めた)描画前処理
		PreDraw();

		//Model-View行列初期化
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_LIGHTING); //GUI関連は照明は無関係
		glColor3f(1, 1, 1);

		//各段階描画
		WINDOW::DrawAll(head.GetDirection()); //非透過窓描画
		stickeies.Each(&DRAWER::Draw);; //視界に貼り付いている物体を描画

		glPushMatrix();
		head.GetView(); //頭の姿勢、位置を反映

#if 1
#if 1
		//動作確認用地面っぽい平面
		glBegin(GL_POLYGON);
		glVertex3f(0, -1.6, -10);
		glVertex3f(8.66, -1.6, 5);
		glVertex3f(-8.66, -1.6, 5);
		glEnd();
#endif
#if 0
		//動作確認マーカー
		for(float z(-2); z <= 2; z += 0.5){
			for(float y(-2); y <= 2; y += 0.5){
				for(float x(-2); x <= 2; x += 0.5){
					float d(sqrt(x*x + y*y + x*x));
					if(d < 0.1){
						continue;
					}
					glPointSize(3 / d);
					glBegin(GL_POINTS);
					glVertex3f(x, y, z);
					glEnd();
				}
			}
		}
#endif
#if 1
		SNOW::DrawAll();
#endif
#endif

		externals.Each(&DRAWER::Draw); //externalを描画

		//透過窓描画
		glPopMatrix(); //窓描画直後の状態に戻す
		glDisable(GL_LIGHTING); //GUI関連は照明は無関係
		WINDOW::DrawTransparentAll(head.GetDirection()); //透過窓描画

		//描画後処理
		PostDraw();
	}
}

void VIEW::SIGCHLD_Handler(int sig){
	pid_t child(0);

	do{
		int ret;
		child = waitpid(-1, &ret, WNOHANG);
	}while(0 < child);

	//終了するよう設定
	Quit();
}



void VIEW::PreDraw(){
	//描画領域設定
	glViewport(0, 0, width, height);

	//投影行列計算、設定
	const float sr(nearDistance / (realDistance * 2));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-realWidth * sr, realWidth * sr,
		-realHeight * sr, realHeight * sr,
		nearDistance, farDistance);
}

void VIEW::PostDraw(){
	Update();
}

