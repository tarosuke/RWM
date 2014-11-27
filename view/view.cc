
#include "../gl/gl.h"

#include <stdio.h>

#include "view.h"
#include "../rift/rift.h"
#include "../widget/window.h"
#include "particleRiver.h"
#include "reference.h"
#include "../image/lightball.h"
#include "welcome.h"
#include "../x/xDisplay.h"


template<> FACTORY<VIEW>* FACTORY<VIEW>::start(0);
TOOLBOX::QUEUE<VIEW::DRAWER> VIEW::stickeies;
TOOLBOX::QUEUE<VIEW::DRAWER> VIEW::externals;
TOOLBOX::QUEUE<VIEW::DRAWER> VIEW::skyboxes;
TOOLBOX::QUEUE<VIEW::DISPLAY> VIEW::xDisplays;
ViewWidget VIEW::widget;
bool VIEW::keep(true);


VIEW& VIEW::New() throw(const char*){
	VIEW* const v(FACTORY<VIEW>::New());
	if(v){
		return *v;
	}

	throw "VRHMDが見つかりませんでした";
}



VIEW::VIEW(unsigned w, unsigned h) : xDisplay(new XDISPLAY(w, h)){
	//基本設定
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	//天箱の色を初期値に
	skyboxColor.r =
	skyboxColor.g =
	skyboxColor.b = 1;
	skyboxColor.a = 0;

	//ウエルカムメッセージ
	new WELCOME(*this, "welcome.png");
#if 0
	//ヘッドトラックチェックなどで使う
	LIGHTBALL lb;
	PARTICLES* const p(new PARTICLESRIVER(64, &lb, 1000, -1, 1, -1, 1, -500, 50, 0, 0, 0.02));
	(*p).SetColor(1, 0.8, 0.7, 0.5);
#endif
	new READY(*this); //起動プログレス終了処理
}

VIEW::~VIEW(){
	if(xDisplay){
		delete xDisplay;
	}
};


void VIEW::Run(){
	//周期処理
	while(keep){
		//Xのイベント処理
		xDisplays.Each(&DISPLAY::Run);

		//頭の向きやイベントに合わせて窓を更新
		glEnable(GL_DEPTH_TEST);
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
		widget.Draw(); //非透過窓描画
		WINDOW::DrawAll(); //非透過窓描画(absolute)

		//頭の向きと位置をModel-View行列に反映
		const COMPLEX<4>::ROTATION r(p.direction);
		const double* const pp(p.position);
		glPushMatrix();
		glRotated(-r.angle * 180 / M_PI, r.axis[0], r.axis[1], r.axis[2]);
		glTranslated(-pp[0], -pp[1], -pp[2]);
		externals.Each(&DRAWER::Draw); //externalを描画

		//天箱は最初のだけ描画
		DRAWER* const sb(skyboxes.Peek());
		if(sb){
			glColor4f(
				skyboxColor.r,
				skyboxColor.g,
				skyboxColor.b,
				skyboxColor.a);
			(*sb).Draw();
		}

#if 0
#if 0
		//動作確認用の地面っぽい平面
		glColor4f(1,1,1,0.2);
		glBegin(GL_POLYGON);
		glVertex3f(0, -1.6, -10);
		glVertex3f(8.66, -1.6, 5);
		glVertex3f(-8.66, -1.6, 5);
		glEnd();
#endif
#if 1
		glColor4f(1,1,1,0.2);
		glLineWidth(7);
		glBegin(GL_LINE_STRIP);
		glVertex3f(-0.2, -0.3, -0.5);
		glVertex3f(0.2, -0.3, -0.5);
		glVertex3f(0.3, -0.1, 0.2);
		glVertex3f(-0.3, -0.1, 0.2);
		glVertex3f(-0.2, -0.3, -0.5);
		glEnd();
#endif
#endif

		externals.Each(&DRAWER::DrawTransparent); //external(透過)を描画

		//透過窓描画
		glPopMatrix(); //窓描画直後の状態に戻す
		glDisable(GL_LIGHTING); //GUI関連は照明は無関係

		widget.DrawTransparent(); //透過窓描画
		WINDOW::DrawTransparentAll(); //透過窓描画(obsolute)

		stickeies.Each(&DRAWER::Draw);; //視界に貼り付いている物体を描画

		//収差修正用参照物体描画
// 		REFERENCE::DrawAll();

		//描画後処理
		PostDraw();

		//VSYNCを待って表示
		xDisplays.Each(&DISPLAY::Update);
	}
}


float VIEW::fov(90);
float VIEW::tanFov(1); //fovが更新されたらtanf(fov * M_PI / 360)で再計算


//初期化五の処理
VIEW::READY::READY(VIEW& v) : fai(0), view(v){
	RegisterStickies(*this);
}
void VIEW::READY::Update(){
	if(fai || 1.0 <= view.InitialProgress()){
		if(faiFrames <= ++fai){
			view.SetSkyboxColor(1,1,1,1);
			delete this;
		}else{
			view.SetSkyboxColor(1,1,1,(float)fai / faiFrames);
		}
	}
}

