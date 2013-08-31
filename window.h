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
#include <toolbox/qon/qon.h>



class WINDOW{
public:
	static void Init();
	static void Run(class GHOST&);
	static void DrawWindows();
private:
	~WINDOW(); //自身をwindowListから削除して消滅
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
	static Atom wInstanceAtom;
	//窓までの距離
	static float baseDistance;
	//窓配置の広がりレシオ(配置＆大きさ0.0〜1.0の角度。単位は°)
	static float spread;

	//単体窓関連
	TOOLBOX::NODE<WINDOW> node;
	bool mapped; //tureならDrawされた時に反応して物体を生成する
	unsigned tID; //窓の内容を転送するテクスチャID

	//中心の位置というか角度(0.0〜1.0がspreadに対応)
	float horiz;
	float vert;
	//角度表記の大きさ(0.0〜1.0がspreadに対応)
	float hSpread;
	float vSpread;
	//ピクセルサイズ
	unsigned width;
	unsigned height;

	//窓生成、登録
	WINDOW(XCreateWindowEvent& e);
	void AssignTexture();
	static WINDOW* FindWindowByDir(const QON&);
	struct P2{
		float x;
		float y;
	};
	P2 GetLocalPosition(const QON&);

	//窓固有のハンドラ
	void OnDamage(XDamageNotifyEvent&);
};


#endif
