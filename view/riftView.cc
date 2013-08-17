/***************************************************** view with rift:riftView
 *
 */
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <assert.h>
#include <stdio.h>

#include "riftView.h"
#include <world/room.h>
#include <rift/rift.h>
#include <avatar/avatar.h>
#include <toolbox/qon/glqon.h>


const char* RIFTVIEW::vertexShaderSource =
"void main(void){"
	"gl_Position = ftransform();"
"}";

const char* RIFTVIEW::fragmentShaderSource =
"uniform sampler2D buffer;"
// "uniform sampler2D de_distor;"
"void main(void){"
	"vec4 dc = gl_FragCoord; dc[3] = 1.0;"
// 	"dc += texture2DProj(de_distor, dc); dc[3] = 1.0;"
	"gl_FragColor = texture2DProj(buffer, dc);"
"}";

int RIFTVIEW::deDistorShaderProgram;
bool RIFTVIEW::glewValid(false);


RIFTVIEW::RIFTVIEW(AVATAR& avatar) :
	VIEW(1280, 800, avatar){

	glewValid = (GLEW_OK == glewInit());
	if(glewValid){
		//プログラマブルシェーダの設定
		GLuint vShader(glCreateShader(GL_VERTEX_SHADER));
		GLuint fShader(glCreateShader(GL_FRAGMENT_SHADER));

		if(!vShader || !fShader){
			glewValid = false;
			return;
		}

		glShaderSource(vShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vShader);

		glShaderSource(fShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fShader);

		deDistorShaderProgram = glCreateProgram();
		glAttachShader(deDistorShaderProgram, vShader);
		glAttachShader(deDistorShaderProgram, fShader);

		GLint linked;
		glLinkProgram(deDistorShaderProgram);
		glGetProgramiv(deDistorShaderProgram, GL_LINK_STATUS, &linked);
		if(GL_FALSE == linked){
			glewValid = false;
			assert(false);
			return;
		}

//		glUseProgram(deDistorShaderProgram);
		glUseProgram(0);

		//歪み情報テクスチャを作る
	}
}

RIFTVIEW::~RIFTVIEW(){}


void RIFTVIEW::Draw() const{
	const float sr(nearDistance / (realDistance * 2));
	const int hw(width / 2);
	const int rhw(realWidth / 2);
	const double inset(0.17);

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

	//TODO:歪み除去
	if(glewValid){
		//フラグメントシェーダによる歪み除去
	}else{
		//ポリゴンタイルによる歪み除去
	}
}

RIFTVIEW::P2 RIFTVIEW::GetTrueCoord(P2 c){
	return c; //TODO:正しい座標を計算する
}

