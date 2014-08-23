#pragma once

#include <X11/Xlib.h>
#include <GL/glx.h>



class XDISPLAY{
	XDISPLAY();
	XDISPLAY(const XDISPLAY&);
	void operator=(const XDISPLAY&);
public:
	XDISPLAY(unsigned w, unsigned h);
	XDISPLAY(const char*);
	~XDISPLAY();
	bool Run();  //falseになったら終了(イベントを受け取るこのクラスが判定)
	void Update(); //描画終了後、VSYNCを待って画面切り替え

private:
	//GL関連
	static int glxAttributes[];
	GLXContext glxContext;

	//エラーハンドリング
	static int damageBase;
	static int damage_err;
	static int XErrorHandler(Display*, XErrorEvent*);

	//画面それ自体の関連
	Display* xDisplay;
	Window rootWindowID;
	unsigned hCenter;
	unsigned vCenter;

	//画面からの入力設定
	void InputSetup();

	//続行フラグ
	static bool keep;

	//X用イベントハンドラと見せかけてRWMのイベントを作るハンドラ
	unsigned modifyState;
	static void AtXKeyUp(const XKeyEvent&);
	static void AtXKeyDown(const XKeyEvent&);
	static void AtXKey(class KEY_EVENT&, const XKeyEvent&);
	static void AtXMouseDown(const XButtonEvent&);
	static void AtXMouseUp(const XButtonEvent&);
	static void AtXMouse(class MOUSE_EVENT&, const XButtonEvent&);
};


