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

private:
	~WINDOW(); //自身をwindowListから削除して消滅
	void Draw(unsigned numFormFront);

	//X関連
	Display* const xDisplay;
	const Window wID; //窓ID
	Damage dID; //xDamageID

	//窓全体関連
	static TOOLBOX::QUEUE<WINDOW> windowList;
	static WINDOW* FindWindowByID(Display*, unsigned wID);
	static bool zoomable;
	static const QON* headDir;
	static const float zoomedScale;

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

	//ピクセル位置、サイズ(scaleを乗じたサイズで描画)
	int vx;
	int vy;
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
	void SeekPosition(
		unsigned hTo, unsigned vTo,
		unsigned hStep, unsigned vStep,
		int gx, int gy);
	unsigned OverLen(int s0, int l0, int s1, int l1);
	unsigned WindowPositionPoint(int x, int y, int gx, int gy);

	//窓固有のハンドラ
	void OnDamage(XDamageNotifyEvent&);
};


#endif
