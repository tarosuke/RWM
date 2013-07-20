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
	static class ROOT{ //TESTの時は窓を開いて、でなければrootを取得
		friend class WINDOW;
	public:
		void Run(class GHOST&);
		void Draw();
	private:
		ROOT();
		~ROOT();
		Display* const xDisplay;
		const int rootWindowID;
		GLXContext glxContext;
		static const int width = 1280;
		static const int height = 800;
	}root;
private:
	int wID; //窓ID
	int tID; //テクスチャID
};

#endif
