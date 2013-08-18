/***************************************************** view with rift:riftView
 *
 */
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "riftView.h"
#include <world/room.h>
#include <rift/rift.h>
#include <avatar/avatar.h>
#include <toolbox/qon/glqon.h>


const char* RIFTVIEW::vertexShaderSource =
"void main(void){"
	"gl_Position = ftransform();"
	"gl_TexCoord[0] = gl_MultiTexCoord0;"
	"gl_TexCoord[1] = gl_MultiTexCoord1;"
"}";

const char* RIFTVIEW::fragmentShaderSource =
"uniform sampler2D buffer;"
"uniform sampler2D de_distor;"
"void main(void){"
	"vec4 dc = gl_TexCoord[0]; dc[3] = 1.0;"
	"dc += texture2DProj(de_distor, gl_TexCoord[0]); dc[3] = 1.0;"
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

		glUseProgram(0);

		//フレームバッファ用テクスチャを確保
		glGenTextures(1, &framebufferTexture);
		glBindTexture(GL_TEXTURE_2D, framebufferTexture);
		glTexParameteri(
			GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(
			GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		assert(glGetError() == GL_NO_ERROR);

		//歪み情報テクスチャを作る
		struct DISTORE_ELEMENT{
			unsigned char u;
			unsigned char v;
		}__attribute__((packed)) *body((DISTORE_ELEMENT*)malloc(width * height * sizeof(DISTORE_ELEMENT)));
		assert(body);
		DISTORE_ELEMENT* b(body);
		for(int v(0); v < height; v++){
			for(int u(0); u < width; u++, b++){
				(*b).u = v;
				(*b).v = 0;
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
		glDisable(GL_LIGHTING);
		glEnable(GL_LIGHTING);
	}
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

RIFTVIEW::P2 RIFTVIEW::GetTrueCoord(P2 c){
	return c; //TODO:正しい座標を計算する
}

