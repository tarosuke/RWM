/************************************************************ view(rootwindow)
 *
 */

#include <stdio.h>

#include "view.h"
#include <window.h>
#include <toolbox/qon/glqon.h>





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
	for(;;){
		//Xのイベント処理
		x.EventHandler();

		//バッファのクリア
		glClear(GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);

		//基本設定
		glEnable(GL_POLYGON_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);

		//基本ライト(場所は自分、明るさはAMBIENTへ)
		glEnable(GL_LIGHT0);
		glLightf(GL_LIGHT0, GL_POSITION, 0);
		const GLfloat myLight[] = { 0.3, 0.3, 0.3, 1 };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, myLight);
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0);

		//フォグ
// 		glFogi(GL_FOG_MODE, GL_LINEAR);
// 		glFogi(GL_FOG_START, 10);
// 		glFogi(GL_FOG_END, 5000);
// 		const GLfloat fogColor[] = { 0.6, 0.6, 0.6, 0.6 };
// 		glFogfv(GL_FOG_COLOR, fogColor);
// 		glHint(GL_FOG_HINT, GL_DONT_CARE);
// 		glEnable(GL_FOG);

		//(投影行列初期化も含めた)描画前処理
		PreDraw();

		//Model-View行列初期化
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_LIGHTING); //GUI関連は照明は無関係

		//各段階描画
		DrawObjects(stickeies); //視界に貼り付いている物体を描画
		GLQON headDir(head.GetDirection());
		headDir.GetView(); //視野を取得
		WINDOW::DrawAll(); //窓を描画
// 		glEnable(GL_LIGHTING); //無効にした照明を有効にする
		DrawObjects(externals); //externalを描画

		//動作確認用
		glBegin(GL_POINTS);
		for(float x(-1.0); x < 1.0; x += 0.1){
			for(float y(-1.0); y < 1.0; y += 0.1){
				glVertex3f(x, y, -1.0);
			}
		}
		glEnd();



		//描画後処理
		PostDraw();
	}
}

void VIEW::DrawObjects(TOOLBOX::QUEUE<DRAWER>& q){
	for(TOOLBOX::QUEUE<DRAWER>::ITOR i(q); i; (*i++).Draw());
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

