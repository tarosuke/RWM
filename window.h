/******************************************************* window handler:window
 * -DTESTの時はrootになるウインドウを開いてそれをroot扱い。
 * でなければrootを取得してそれをrootとする。
 * rootに対してXCompositRedirectSubWindowsして内容を取得
 * また、窓に対してxdamegeイベントを受け付けるよう設定。
 */
#ifndef _WINDOW_
#define _WINDOW_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xdamage.h>

#include <GL/gl.h>
#include <GL/glx.h>

#include <toolbox/queue/queue.h>



class WINDOW{
public:
	static void Init();
	static void Run(class GHOST&);
	static void DrawWindows();
protected:
	WINDOW(); //自分でXCreateWindowする
	virtual ~WINDOW(); //自身をwindowListから削除して消滅
	void Draw(unsigned numFormFront);
	static Display* xDisplay;
	static unsigned rootWindowID;
	static GLXContext glxContext;
	static int rootWidth;
	static int rootHeight;

	unsigned wID; //窓ID
	Damage dID;
	static int damageBase;
	static int damage_err;
private:
	//根窓関連
	static void Quit();
	static void AtCreate(XCreateWindowEvent&);
	static void AtMap(XMapEvent&);
	static void AtDestroy(XDestroyWindowEvent&);
	static void AtUnmap(XUnmapEvent&);
	static void AtMapping(XMappingEvent&){};
	static void AtDamage(XDamageNotifyEvent&);

	static void DrawAll();
	static void HandleXEvent(XEvent&);

	//窓全体関連
	static TOOLBOX::QUEUE<WINDOW> windowList;
	static WINDOW* FindWindowByID(unsigned wID);
	static int XErrorHandler(Display*, XErrorEvent*);

	//単体窓関連
	TOOLBOX::NODE<WINDOW> node;
	bool mapped; //tureならDrawされた時に反応して物体を生成する
	unsigned tID; //窓の内容を転送するテクスチャID

	//窓までの距離
	static float baseDistance;
	//窓の広がり(horizやvertを掛けて角度を決める。単位は°)
	static float horizAngle;
	static float vertAngle;
	//位置というか角度(普通は-0.5〜0.5に正規化されている)
	float horiz;
	float vert;
	//ピクセルサイズ
	unsigned width;
	unsigned height;

	WINDOW(int wID); //wIDを持つWINDOWを生成、windowListへ追加
	void AssignTexture();

	//窓固有のハンドラ
	void OnDamage(XDamageNotifyEvent&);
};

//RWM側で生成した窓
class INTERNAL_WINDOW : public WINDOW{
public:
	~INTERNAL_WINDOW(){
		XDestroyWindow(xDisplay, wID);
	};
};

#endif
