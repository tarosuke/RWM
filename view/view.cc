
#include <GL/glew.h>
#include <GL/gl.h>

#include "view.h"
#include "../rift/rift.h"
#include "../window/window.h"
#include "snow.h"



template<> FACTORY<VIEW>* FACTORY<VIEW>::start(0);
TOOLBOX::QUEUE<VIEW::DRAWER> VIEW::stickeies;
TOOLBOX::QUEUE<VIEW::DRAWER> VIEW::externals;


VIEW& VIEW::New() throw(const char*){
	VIEW* const v(FACTORY<VIEW>::New());
	if(v){
		return *v;
	}

	throw "VRHMDが見つかりませんでした";
}



VIEW::VIEW(unsigned w, unsigned h) : xDisplay(w, h){
	//基本設定
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}




void VIEW::Run(){
	//周期処理
	while(xDisplay.Run()){
		//頭の向きやイベントに合わせて窓を更新
		const POSE p(Pose());
		WINDOW::UpdateAll(p.direction);
		stickeies.Each(&DRAWER::Update);
		externals.Each(&DRAWER::Update);

		//バッファのクリア
		glClear(GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);

		//(投影行列初期化も含めた)描画前処理
		PreDraw();

		glDisable(GL_LIGHTING); //GUI関連は照明は無関係
		glColor3f(1, 1, 1);

		//各段階描画
		WINDOW::DrawAll(); //非透過窓描画

		//頭の向きと位置をModel-View行列に反映
		const COMPLEX<4>::ROTATION r(p.direction);
		const double* const pp(p.position);
		glPushMatrix();
		glRotated(-r.angle * 180 / M_PI, r.axis[0], r.axis[1], r.axis[2]);
		glTranslated(-pp[0], -pp[1], -pp[2]);

		externals.Each(&DRAWER::Draw); //externalを描画

#if 1
		//動作確認用の地面っぽい平面
		glColor4f(1,1,1,1);
		glBegin(GL_POLYGON);
		glVertex3f(0, -1.6, -10);
		glVertex3f(8.66, -1.6, 5);
		glVertex3f(-8.66, -1.6, 5);
		glEnd();
#if 1
		SNOW::DrawAll();
#endif
#endif

		//透過窓描画
		glPopMatrix(); //窓描画直後の状態に戻す
		glDisable(GL_LIGHTING); //GUI関連は照明は無関係
		WINDOW::DrawTransparentAll(); //透過窓描画
		stickeies.Each(&DRAWER::Draw);; //視界に貼り付いている物体を描画

		//描画後処理
		PostDraw();

		//VSYNCを待って表示
		xDisplay.Update();

	}
}


float VIEW::fov(90);
float VIEW::tanFov(1); //fovが更新されたらtanf(fov * M_PI / 360)で再計算
