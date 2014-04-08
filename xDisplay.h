/***************************************************************** X eventloop
 *
 */
#ifndef _X_
#define _X_

#include <pthread.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xcomposite.h>

#include <GL/gl.h>
#include <GL/glx.h>

#include <window.h>


class XDISPLAY{
public:
	XDISPLAY();
	virtual ~XDISPLAY();
	void Update(){
		glXSwapBuffers(xDisplay, rootWindowID);
	};
	void EventHandler();

	//WINDOWのためのアクセサ
	int Width(){ return width; };
	int Height(){ return height; };
	Display* XDisplay() const{ return xDisplay; };
	Window Root(){ return rootWindowID; };
protected:
	XDISPLAY(Display*);
	void Setup();
	void SetupGL();

	//根窓 & GLX関連
	Display* xDisplay;
	Window rootWindowID;
	unsigned width;
	unsigned height;
	unsigned hCenter;
	unsigned vCenter;
	static int XErrorHandler(Display*, XErrorEvent*);
	pthread_t evThread;
	GLXContext glxContext;
	void Activate(){
		//このVIEWの描画条件をカレントにする
		glXMakeCurrent(xDisplay, rootWindowID, glxContext);
	};

	//XDamage関連
	static int damageBase;
	static int damage_err;

	//内部イベントハンドラ
	void AtDamage(XDamageNotifyEvent&);

	static void AtMouse(WINDOW::EVENT::EVENT_TYPE, const XButtonEvent&);


	//テスト窓
#ifdef TEST
	Window testWindow;
	void TestWindow();
#endif
};

//Xvfbを起動し、イベントループは別スレッド
class XVFB : public XDISPLAY{
public:
	XVFB();
private:
	Display* StartFB(); //Xvfbに接続し、xsmを起動する
};



#endif
