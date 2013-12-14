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



class XDISPLAY;
class WINDOW{
	WINDOW();
public:
	WINDOW(XCreateWindowEvent& e, XDISPLAY&, bool mapState = false);

	static void DrawAll(const QON& headDir);

	//根窓関連
	static void AtCreate(XCreateWindowEvent&, XDISPLAY&);
	static void AtMap(XMapEvent&);
	static void AtDestroy(XDestroyWindowEvent&);
	static void AtUnmap(XUnmapEvent&);
	static void AtDamage(XEvent&);
	static void AtKeyEvent(XEvent&);
	static void AtMappingEvent(XMappingEvent&);
	static void AtButtonEvent(XButtonEvent&);
	static void AtConfigureEvent(XConfigureEvent&);

private:
	~WINDOW(); //自身をwindowListから削除して消滅
	void Draw(unsigned numFormFront);

	//X関連
	XDISPLAY& display;
	const Window wID; //窓ID
	Damage dID; //xDamageID

	//窓全体関連
	static TOOLBOX::QUEUE<WINDOW> windowList;
	static WINDOW* FindWindowByID(Display*, unsigned wID);
	static bool zoomable; //ズーム処理する窓をひとつだけにするためのフラグ
	static const QON* headDir;
	static const float zoomedScale; //ズームした時の大きさ[mm/px]
	static WINDOW* focused;
	void Focus();
	void UnFocus();
	void See(int x, int y);
	static int seenX;
	static int seenY;

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

	void Moved(int x, int y); //仮想空間側の窓だけ移動
	void Move(int x, int y); //X側の窓も移動
	void Resized(unsigned w, unsigned h); //仮想空間側の窓をリサイズ、テクスチャ再設定
	void Resize(unsigned w, unsigned h); //X側の窓だけリサイズ(仮想空間側はXConfigureEventにて追随)

	//ピクセル位置、サイズ(scaleを乗じたサイズで描画)
	int vx;
	int vy;
	unsigned width;
	unsigned height;

	//窓生成、登録
	void AssignTexture();
	struct P2{
		float x;
		float y;
	};
	P2 GetLocalPosition(const QON&);
	void SeekPosition(
		int hTo, int vTo,
		int hStep, int vStep,
		int gx, int gy);
	unsigned OverLen(int s0, int l0, int s1, int l1);
	unsigned WindowPositionPoint(int x, int y, int gx, int gy);

	//窓固有のハンドラ
	void OnDamage(XDamageNotifyEvent&);
};


#endif
