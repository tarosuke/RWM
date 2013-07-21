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

#include <GL/gl.h>
#include <GL/glx.h>

#include <toolbox/queue/queue.h>



class WINDOW{
public:
	class ROOT{ //TESTの時は窓を開いて、でなければrootを取得
		friend class WINDOW;
	public:
		ROOT();
		~ROOT();
		void Run(class GHOST&);
	private:
		Display* const xDisplay;
		const unsigned rootWindowID;
		GLXContext glxContext;
		static const int width = 1280;
		static const int height = 800;
		void Draw();
		void AtCreate(XCreateWindowEvent&);
		void AtMap(XMapEvent&);
		void AtDestroy(XDestroyWindowEvent&);
		void AtUnmap(XUnmapEvent&);
	};
	static void DrawAll();
protected:
	virtual ~WINDOW(); //自身をwindowListから削除して消滅
// 	virtual void Draw();
private:
	static TOOLBOX::QUEUE<WINDOW> windowList;
	static WINDOW* FindWindowByID(unsigned wID);
	unsigned wID; //窓ID
	int tID; //テクスチャID
	bool mapped; //tureならDrawされた時に反応して物体を生成する
	TOOLBOX::NODE<WINDOW> node;

	unsigned width;
	unsigned height;

	WINDOW(int wID); //wIDを持つWINDOWを生成、windowListへ追加
};

#endif
