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

#include <view/view.h>



class WINDOW{
	WINDOW();
public:
	WINDOW(XCreateWindowEvent& e, unsigned rootWidth, unsigned rootHeight);

	static void DrawAll(const QON& headDir);


	//根窓関連
	static void AtCreate(XCreateWindowEvent&, unsigned rw, unsigned rh);
	static void AtMap(XMapEvent&);
	static void AtDestroy(XDestroyWindowEvent&);
	static void AtUnmap(XUnmapEvent&);
	static void AtMapping(XMappingEvent&){};
	static void AtDamage(XDamageNotifyEvent&);

	//窓関連
	static WINDOW* FindWindowByDir(const QON&);

private:
	~WINDOW(); //自身をwindowListから削除して消滅
	void Draw(unsigned numFormFront);

	//X関連
	Display* const xDisplay;
	const unsigned wID; //窓ID
	Damage dID; //xDamageID

	//窓全体関連
	static TOOLBOX::QUEUE<WINDOW> windowList;
	static WINDOW* FindWindowByID(Display*, unsigned wID);
	static bool zoomable;
	static const QON* headDir;

	//単体窓関連
	TOOLBOX::NODE<WINDOW> node;
	bool mapped; //tureならDrawされた時に描画する
	unsigned tID; //窓の内容を保持するテクスチャID

	//中心の位置というか角度
	float horiz; //水平角(°)
	float vert; //垂直角(°)
	float scale; //非ズーム時の大きさ(ズーム時はピクセル基準)
	float distance; //最後に描画した時の距離
	static float baseDistance;
	QON center; //中心の向き

	void Move(int x, int y);

	//ピクセルサイズ(scaleを乗じたサイズで描画)
	unsigned width;
	unsigned height;
	unsigned rootWidth;
	unsigned rootHeight;

	//窓生成、登録
	void AssignTexture();
	struct P2{
		float x;
		float y;
	};
	P2 GetLocalPosition(const QON&);

	//窓固有のハンドラ
	void OnDamage(XDamageNotifyEvent&);
};


#endif
