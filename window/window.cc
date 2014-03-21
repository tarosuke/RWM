/******************************************************* window handler:window
 */
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <X11/Xlib.h>

#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <window.h>
#include <view/view.h>
#include <toolbox/cyclic/cyclic.h>



//窓までの距離
float WINDOW::baseDistance(0.5);

//窓全体制御関連
TOOLBOX::QUEUE<WINDOW> WINDOW::windowList;
WINDOW* WINDOW::focused(0);
const float WINDOW::scale(0.0007);



//窓生成
WINDOW::WINDOW() :
	node(windowList),
	tID(0),
	visibility(false){
}


WINDOW(float h, float v, const IMAGE& initialImage) :
	node(windowList),
	tID(0),
	visibility(true){

	//テクスチャ割り当て
	AssignImage(initialImage);
}


// テクスチャ割り当て
void AssignImage(const IMAGE& image){
	//テクスチャが既に割り当てられていたら入れ替えるために解放
	if(tID){
		glDeleteTextures(&tID);
	}

	//サイズ取得
	width = image.GetWidth();
	height = image.GetHeight();

	//テクスチャ割り当て
	glGenTextures(1, &tID);
	assert(tID);

	glBindTexture(GL_TEXTURE_2D, tID);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	void* const image(image.GetmemoryImage());
	if(image){
		//テクスチャへ転送
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB, w, h, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, image);
	}else{
		//TODO:仮データを割り当てておく
		assert(false);
	}

	//テクスチャの属性を設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void WINDOW::UpdateImage(
	const IMAGE& image, //元イメージ
	unsigned dx, //書き込み先座標
	unsigned dy,
	unsigned w, //転送サイズ
	unsigned h{
	if(!tID){
		//テクスチャなし
		return;
	}
	glBindTexture(GL_TEXTURE_2D, tID);
	glTexSubImage2D(
		GL_TEXTURE_2D, 0,
		e.area.x,
		e.area.y,
		w,
		h,
		GL_BGRA,
		GL_UNSIGNED_BYTE,
		image.GetmemoryImage());
	glBindTexture(GL_TEXTURE_2D, 0);
	XDestroyImage(wImage);

}

void WINDOW::UpdateImage(
	const IMAGE& image, //元イメージ
	unsigned sx, //元イメージ上の座標
	unsigned sy,
	unsigned dx, //書き込み先座標
	unsigned dy,
	unsigned w, //転送サイズ
	unsigned h{
	if(!tID){
		//テクスチャなし
		return;
	}

	//サブイメージ取得
	const IMAGE subImage(image, sx, dx, w, h);

	//テクスチャアップデート
	UpdateImage(subImage, w, h);
}

WINDOW::~WINDOW(){
	UnFocus();
	if(tID){
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &tID);
	}
}

void WINDOW::Focus(){
	if(focused){
		(*focused).OnUnfocus();
	}
	if(this != focused){
		focused = this;
		OnFocused();
	}
}

void WINDOW::UnFocus(){
	if(this == focused){
		OnUnfocus();
		focused = 0;
	}
}

void WINDOW::Draw(float xoff, float yoff, float distance){
	//可視設定チェック
	if(!visibility){
		return;
	}

	//描画位置算出
	const float h(horiz - xoff);
	const float v(vert - yoff);

	//向きと位置を設定
	glPushMatrix();
	glRotatef(-(h / distance) * 180 / M_PI, 0, 1, 0);
	glRotatef(-(v / distance) * 180 / M_PI, 1, 0, 0);

	//描画
	glBindTexture(GL_TEXTURE_2D, tID);
	glBegin(GL_TRIANGLE_STRIP);
	const float w(width * scale * 0.5);
	const float h(height * scale * 0.5);
	glTexCoord2f(0, 0);
	glVertex3f(-w, h, -distance);
	glTexCoord2f(0, 1);
	glVertex3f(-w, -h, -distance);
	glTexCoord2f(1, 0);
	glVertex3f(w, h, -distance);
	glTexCoord2f(1, 1);
	glVertex3f(v, -h, -distance);
	glEnd();

	//後始末
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}


