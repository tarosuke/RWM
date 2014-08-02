
#include <GL/glew.h>
#include <GL/gl.h>

#include "view.h"
#include "rift/rift.h"



template<> FACTORY<VIEW>* FACTORY<VIEW>::start(0);


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
		//バッファのクリア
		glClear(GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);

		//(投影行列初期化も含めた)描画前処理
		PreDraw();

		//Model-View行列初期化
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_LIGHTING); //GUI関連は照明は無関係
		glColor3f(1, 1, 1);

		//各段階描画
		//WINDOW::DrawAll(head.GetDirection()); //非透過窓描画
		stickeies.Each(&DRAWER::Draw);; //視界に貼り付いている物体を描画

		//頭の向きと位置をModel-View行列に反映
		const POSE p(Pose());
		COMPLEX<4>::ROTATION r;
		p.direction.GetRotation(r);
		glPushMatrix();
		const double* const pp(p.position);
		glTranslated(-pp[0], -pp[1], -pp[2]);
		glRotated(-r.angle * 180 / M_PI, r.axis[0], r.axis[1], r.axis[2]);

		externals.Each(&DRAWER::Draw); //externalを描画

		//透過窓描画
		glPopMatrix(); //窓描画直後の状態に戻す
		glDisable(GL_LIGHTING); //GUI関連は照明は無関係
		//WINDOW::DrawTransparentAll(head.GetDirection()); //透過窓描画

		//描画後処理
		PostDraw();
	}
}


