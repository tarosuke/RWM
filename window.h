
#ifndef _WINDOW_
#define _WINDOW_


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "list.h"



class WINDOW : public LIST<WINDOW>::NODE{
public:
	class POINT{
	public:
		float h;
		float v;
	};
	static WINDOW* FindWindow(float h, float v);
	WINDOW() : xWindow(-1){};
	WINDOW(XCreateWindowEvent&);
	static void AtPointed(POINT&, XEvent&);
	static void AtFocused(XEvent&);
	static void Update(){
		for(LIST<WINDOW>::ITOR i(list); i; i++){
			(*i).Draw();
		}
	};
	static void MapIt(XMapEvent& xe){
		for(LIST<WINDOW>::ITOR i(list); i; i++){
			if((*i).xWindow == xe.window){
				(*i).Map(xe);
				break;
			}
		}
	};
private:
	static LIST<WINDOW> list;
	const Window xWindow;	//Xの窓ID
	bool mapped;		//マップされたらtrue
	/** 配置はユーザを中心とした極座標。
	 * 窓はそれ以外の配置方法はない
	 * そもそも窓は陰とか付けない特別扱い
	 * 画像などは「背景」にオブジェクトとして配置はできる(予定)
	 */
	float horiz;	//中心の水平角
	float virt;	//中心の仰角
	float distance;	//視点からの距離
	float width;	//幅(物理量)
	float height;	//高さ(物理量)
	bool IsIn(float h, float v);
	POINT LocalCoords(POINT direction);
	void Draw();
	void Map(XMapEvent&);
};


#endif
