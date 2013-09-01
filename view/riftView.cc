/***************************************************** view with rift:riftView
 *
 */
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "riftView.h"
#include <world/room.h>
#include <rift/rift.h>
#include <avatar/avatar.h>
#include <toolbox/qon/glqon.h>


#define DISTORFIX 0

extern "C"{
extern char _binary_vertex_pss_start[];
extern char _binary_fragment_pss_start[];
extern char _binary_vertex_pss_end[];
extern char _binary_fragment_pss_end[];
};
const char* RIFTVIEW::vertexShaderSource(_binary_vertex_pss_start);
const char* RIFTVIEW::fragmentShaderSource(_binary_fragment_pss_start);

int RIFTVIEW::deDistorShaderProgram;
bool RIFTVIEW::glewValid(false);


RIFTVIEW::RIFTVIEW(AVATAR& avatar) :
	VIEW(1280, 800, avatar){

	glewValid = (GLEW_OK == glewInit());
	if(glewValid){
		//シェーダーコードの整形
		_binary_fragment_pss_end[-1] =
		_binary_vertex_pss_end[-1] = 0;

		//プログラマブルシェーダの設定
		GLuint vShader(glCreateShader(GL_VERTEX_SHADER));
		GLuint fShader(glCreateShader(GL_FRAGMENT_SHADER));

		if(!vShader || !fShader){
			glewValid = false;
			return;
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
			glewValid = false;
			assert(false);
			return;
		}
		assert(glGetError() == GL_NO_ERROR);

		glUseProgram(0);

		//フレームバッファ用テクスチャを確保
		glGenTextures(1, &framebufferTexture);
		glBindTexture(GL_TEXTURE_2D, framebufferTexture);
		glTexParameteri(
			GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(
			GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(
			GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(
			GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// 		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

#if DISTORFIX
		//歪みチェック用テクスチャ生成
		struct DISTORE_CHECKER{
			unsigned char r;
			unsigned char g;
			unsigned char b;
		}__attribute__((packed)) *checker((DISTORE_CHECKER*)malloc(width * height * sizeof(DISTORE_CHECKER)));
		for(int v(0); v < height; v++){
			for(int u(0); u < width; u++){
				DISTORE_CHECKER& b(checker[v*width + u]);
				b.r = b.g = b.b = !(u%10) || !(v%10) ? 255 : 0;
			}
		}
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB,
			width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, checker);

#endif

		assert(glGetError() == GL_NO_ERROR);

		//歪み情報テクスチャを作る
		struct DISTORE_ELEMENT{
			unsigned char u;
			unsigned char v;
		}__attribute__((packed)) *body((DISTORE_ELEMENT*)malloc(width * height * sizeof(DISTORE_ELEMENT)));
		assert(body);
		for(int v(0); v < height; v++){
			for(int u(0); u < width / 2; u++){
				DISTORE_ELEMENT& b(body[v*width + u]);
				DISTORE_ELEMENT& d(body[v*width + width - u - 1]);
#if DISTORFIX
				if((320 <= u && u < 325) ||
				   (400 <= v && v < 405)){
					//目盛
					b.u = d.u = b.v = d.v = 128;
				}else{
#endif
					P2 tc(GetTrueCoord(u, v));
					const float uu(tc.u - u);
					const float vv(tc.v - v);
					if(tc.u < 0.0 || width / 2 <= tc.u ||
					   tc.v < 0.0 || height <= tc.v ||
					   uu < -127 || 127 < uu ||
					   vv < -127 || 127 < vv){
						b.u = d.u = b.v = d.v = 0;
					}else{
						b.u = uu + 128;
						d.u = -uu + 128;
						b.v =
						d.v = vv + 128;
					}
#if DISTORFIX
				}
#endif
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
			GL_TEXTURE_2D, 0, GL_RG,
			width, height, 0, GL_RG, GL_UNSIGNED_BYTE, body);
		free(body);
		assert(glGetError() == GL_NO_ERROR);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

RIFTVIEW::~RIFTVIEW(){
	glDeleteTextures(1, &framebufferTexture);
	glDeleteTextures(1, &deDistorTexture);
}


void RIFTVIEW::Draw() const{
	const float sr(nearDistance / (realDistance * 2));
	const int hw(width / 2);
	const int rhw(realWidth / 2);
	const double inset(0.1453);

	//左目
	glViewport(0, 0, hw, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum((-rhw - inset) * sr, (rhw - inset) * sr,
			-realHeight * sr, realHeight * sr,
	nearDistance, farDistance);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.03, 0, 0);
	avatar.GetView();
	static bool drawn(false);
	if(!drawn){
		glNewList(displayList, GL_COMPILE_AND_EXECUTE);
		DrawForEye();
		glEndList();
		// 			drawn = true;
	}else{
		glCallList(displayList);
	}

	//右目
	glViewport(hw, 0, hw, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum((-rhw + inset) * sr, (rhw + inset) * sr,
			-realHeight * sr, realHeight * sr,
	nearDistance, farDistance);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.03, 0, 0);
	avatar.GetView();
	glCallList(displayList);

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
	if(glewValid){
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
	}else{
		//ポリゴンタイルによる歪み除去
// 		glDisable(GL_LIGHTING);
// 		glEnable(GL_LIGHTING);
	}
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

float RIFTVIEW::D(float dd){
	return 1.0 +
		0.35 * dd +
		0.125 * dd*dd +
		0.075 * dd*dd*dd;
}

RIFTVIEW::P2 RIFTVIEW::GetTrueCoord(float u, float v){
	const P2 lens = { (float)1.1453 * width/4, (float)height / 2 };

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

