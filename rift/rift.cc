#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdio.h>

#include "rift.h"


void RIFT::UpdateAngularVelocity(const int angles[3], double dt){
	COMPLEX<4> delta(angles, 0.0001 * dt);
	pose.direction *= delta;
}

void RIFT::UpdateAccelaretion(const int axis[3], double dt){
	//加速度情報取得
	VECTOR<3> acc(axis, 0.0001);

	//重力加速度分離
	VECTOR<3> g(acc);
	const double ratio(1.0 / averageRatio);
	g *= ratio;
	gravity *= 1.0 - ratio;
	gravity += g;

	//重力を除去
	acc -= gravity;

	//位置や速度を求める
	if(dt <= 0.02){ //dtが異常に大きい時はデータが欠損しているので位置更新しない
		acc.Rotate(pose.direction); //絶対座標系へ変換
		acc *= dt;
		velocity += acc;
		VECTOR<3> v(velocity);
		v *= dt;
		pose.position += v;

		//枠を出ていたら止める
		const float limit(0.4);
		double* p(pose.position);
		double* ve(velocity);
		for(unsigned n(0); n < 3; ++n){
			if(p[n] < - limit || limit < p[n]){
				ve[n] = 0;
				p[n] = p[n] < 0 ? -limit : limit;
			}
		}
		velocity *= 0.999;
		pose.position *= 0.99;
	}
}

void RIFT::UpdateMagneticField(const int axis[3]){
	//磁力情報取得
	VECTOR<3> mag(axis);

	//キャリブレーション
	if(!magReady){
		//磁化情報をある程度得たらそれ以上は更新しない。
		magMax.Max(mag);
		magMin.Min(mag);
	}
	const VECTOR<3> deGain(magMax - magMin);
	const double* const d(deGain);

	if(magReady){
		//磁化分を除去
		VECTOR<3> offset(magMax + magMin);
		offset *= 0.5;
		mag -= offset;

		//各軸ゲイン調整
		// 		double* const g(mag);
		// 		g[0] /= d[0];
		// 		g[1] /= d[1];
		// 		g[2] /= d[2];
		mag.Normalize();

		//平均化処理
		const double r(1.0 / averageRatio);
		mag *= r;
		// const double* const h(mag);
		// printf("magRatio:%u %lf %lf %lf.\n", magAverageRatio, h[1], h[2], h[3]);
		magneticField *= 1.0 - r;
		magneticField += mag;;
	}else{
		//キャリブレーション可能判定
		if(7000 < abs(d[0]) && 7000 < abs(d[1]) && 7000 < abs(d[2])){
			magReady = true;
			averageRatio = initialAverageRatio;
			puts("magnetic azimuth correction READY.");
		}
	}
}



/////VIEW関連

extern "C"{
	extern char _binary_rift_vertex_glsl_start[];
	extern char _binary_rift_fragment_glsl_start[];
	extern char _binary_rift_vertex_glsl_end[];
	extern char _binary_rift_fragment_glsl_end[];
};
const char* RIFT::vertexShaderSource(_binary_rift_vertex_glsl_start);
const char* RIFT::fragmentShaderSource(_binary_rift_fragment_glsl_start);

#define MaxFloat (3.40282347e+38F)
#define G (9.80665)
RIFT::RIFT(unsigned w, unsigned h) :
	VIEW(w, h),
	averageRatio(initialAverageRatio),
	gravity((const double[]){ 0.0, -G, 0.0 }),
	magMax((const double[]){ -MaxFloat, -MaxFloat, -MaxFloat }),
	magMin((const double[]){ MaxFloat, MaxFloat, MaxFloat }),
	magFront((const double[]){ 0, 0, 1 }),
	magReady(false),
	magneticField((const double[3]){ 0.0, 0.0, 0.01 }),
	width(w),
	height(h){

	if(GLEW_OK != glewInit()){
		throw "GLEWが使えません";
	}
	//シェーダーコードの整形
	_binary_rift_fragment_glsl_end[-1] =
	_binary_rift_vertex_glsl_end[-1] = 0;

	//プログラマブルシェーダの設定
	GLuint vShader(glCreateShader(GL_VERTEX_SHADER));
	GLuint fShader(glCreateShader(GL_FRAGMENT_SHADER));

	if(!vShader || !fShader){
		throw "シェーダの確保に失敗";
	}

	glShaderSource(vShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vShader);

	assert(glGetError() == GL_NO_ERROR);

	glShaderSource(fShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fShader);

	assert(glGetError() == GL_NO_ERROR);

	deDistorShaderProgram = glCreateProgram();
	glAttachShader(deDistorShaderProgram, vShader);
	glAttachShader(deDistorShaderProgram, fShader);

	assert(glGetError() == GL_NO_ERROR);

	GLint linked;
	glLinkProgram(deDistorShaderProgram);
	glGetProgramiv(deDistorShaderProgram, GL_LINK_STATUS, &linked);
	if(GL_FALSE == linked){
		throw "シェーダのリンクに失敗";
	}
	assert(glGetError() == GL_NO_ERROR);

	glUseProgram(0);

	//フレームバッファ用テクスチャを確保
	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(
		GL_TEXTURE_2D,
		GL_TEXTURE_BORDER_COLOR,
		(const float[]){ 0, 0, 0, 1 });
	assert(glGetError() == GL_NO_ERROR);

	//歪み情報テクスチャを作る
	struct DISTORE_ELEMENT{
		float u;
		float v;
	}__attribute__((packed)) *body(
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


float RIFT::D(float dd){
	return 1.0 +
	0.35 * dd +
	0.125 * dd*dd +
	0.075 * dd*dd*dd;
}

RIFT::P2 RIFT::GetTrueCoord(float u, float v){
	const P2 lens = { (1 + inset) * width/4, (float)height / 2 };

	//レンズ位置からの相対座標へ変換
	const P2 l = { u - lens.u, v - lens.v };

	//正規化された距離の自乗
	const float dd((l.u*l.u + l.v*l.v)/(lens.u*lens.u));

	//変換された距離
	const float ddd(D(dd)/* / D(1)*/);

	//結果格納
	const P2 tc = { lens.u + l.u * ddd, lens.v + l.v * ddd };

	return tc;
}


void RIFT::PreDraw(){
	const float tf(GetTanFov());
	const int hw(width / 2);
	const float ar((float)width / height);

	//左目
	glViewport(0, 0, hw, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum((-ar - inset) * tf , (ar - inset) * tf, -tf, tf,
		nearDistance, farDistance);

	//記録と描画
	displayList.StartRecord(true);
}

void RIFT::PostDraw(){
	const float tf(GetTanFov());
	const int hw(width / 2);
	const float ar((float)width / height);

	displayList.EndRecord(); //記録完了

	//右目
	glViewport(hw, 0, hw, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum((-ar + inset) * tf , (ar + inset) * tf, -tf, tf,
		  nearDistance, farDistance);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.03, 0, 0);
	displayList.Playback();

	//Riftの歪み除去
	glGetError();
	glViewport(0, 0, width, height);
	assert(glGetError() == GL_NO_ERROR);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	assert(glGetError() == GL_NO_ERROR);
	#if !DISTORFIX
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);
	#endif
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
}
