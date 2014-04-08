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
	static void AtXCreate(const XCreateWindowEvent&, unsigned, unsigned);
	static void AtXCreate(const Display*, Window, unsigned, unsigned);
	static void AtXDestroy(const XDestroyWindowEvent&);
	static void AtXMap(const XMapEvent&);
	static void AtXUnmap(const XUnmapEvent&);
	static void AtXDamage(const XEvent&);
	static void AtXKey(const XKeyEvent&);
	static void AtXConfigure(const XConfigureEvent&);

private:
	~XWINDOW(){}; //Xのメッセージに追従し、外部からは呼ばれないのでprivate

	//X関連
	const Display* const display;
	const Window wID; //窓ID
	Damage dID; //xDamageID

	//X側の位置メモ
	int vx;
	int vy;
	int xCenter;
	int yCenter;

	//窓全体関連
	static TOOLBOX::QUEUE<XWINDOW> xWindowList; //X窓リスト
	static XWINDOW* FindWindowByID(const Display*, Window);

	//単体窓関連
	TOOLBOX::NODE<XWINDOW> xNode; //X窓リストのノード

	void AssignXTexture();



	//内部イベントハンドラ
	void OnMoved(int x, int y); //X側で窓が移動した
	void OnResized(unsigned w, unsigned h); //X側でリサイズされた

	//窓固有のハンドラ
	void OnDamage(XDamageNotifyEvent&);
	void OnKeyDown(const KEY_EVENT&);
	void OnKeyUp(const KEY_EVENT&);
	void OnMouseDown(const MOUSE_EVENT&);
	void OnMouseUp(const MOUSE_EVENT&);
	void OnMouseEnter(const MOUSE_EVENT&);
	void OnMouseLeave(const MOUSE_EVENT&);
	void OnMouseMove(const MOUSE_EVENT&);
};

