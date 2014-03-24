/************************************************** X11 window handler:xWindow
 */
#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xdamage.h>

#include <GL/gl.h>
#include <GL/glx.h>

#include <toolbox/queue/queue.h>

#include <window.h>



class XDISPLAY;
class XWINDOW : public WINDOW{
	XWINDOW();
public:
	XWINDOW(float x, float y, int w, int h, Window, const Display*);

	//X用イベントハンドラ
	static void AtXCreate(const XCreateWindowEvent&);
	static void AtXCreate(const Display*, Window);
	static void AtXDispose();

private:
	~XWINDOW(){}; //Xのメッセージに追従し、外部からは呼ばれないのでprivate

	//X関連
	const Display* const display;
	const Window wID; //窓ID
	Damage dID; //xDamageID

	//窓全体関連
	static TOOLBOX::QUEUE<XWINDOW> xWindowList; //X窓リスト
	static XWINDOW* FindWindowByID(const Display*, Window);

	//単体窓関連
	TOOLBOX::NODE<XWINDOW> xNode; //X窓リストのノード

	void AssignXTexture();




	void Moved(int x, int y); //仮想空間側の窓だけ移動
	void Move(int x, int y); //X側の窓も移動
	void Resized(unsigned w, unsigned h); //仮想空間側の窓をリサイズ、テクスチャ再設定
	void Resize(unsigned w, unsigned h); //X側の窓だけリサイズ(仮想空間側はXConfigureEventにて追随)

	//窓生成、登録

	//窓固有のハンドラ
	void OnDamage(XDamageNotifyEvent&);
};

