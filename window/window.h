/******************************************************* window handler:window
 * この窓はXの窓ではない。
 * Xの窓には画面という概念があるが、この窓には窓があるだけで画面はない。
 * また、このクラスはバッキングストレージはテクスチャとしてしか持っていない。
 */
#pragma once

#include <string>

#include "../toolbox/queue/queue.h"
#include "../toolbox/complex/complex.h"
#include "../view/view.h"
#include "../gl/texture.h"



class WINDOW{
public:
	class EVENT{
		EVENT();
		EVENT(const EVENT&);
		void operator=(const EVENT&);
	public:
		time_t timestamp;
		static const unsigned ShiftKey = 1;
		static const unsigned CtrlKey = 2;
		static const unsigned AltKey = 4;
		unsigned modifiers; //モディファイアキーの状態
		const void* orgEvent;

		EVENT(unsigned mod, const void* oe);
		virtual ~EVENT();

		virtual void Handle()const=0;
	};
	class MOUSE_EVENT : public EVENT{
	public:
		float x; //窓内相対
		float y;
		float hScroll; //水平スクロール量
		float vScroll; //垂直スクロール量
		unsigned button; //操作されたボタン
		unsigned buttonState; //ボタンの状態
		unsigned clicks; //クリック回数(動いたり違うボタンでクリア)
		WINDOW* prevWindow; //Enterした時に直前にLeaveした窓(それ以外は無意味)
	};
	class KEY_EVENT : public EVENT{
		unsigned charCode; //文字コード
		unsigned keyCode; //キーコード(あれば。なければ0)
	};
	class JS_EVENT : public EVENT{
		unsigned upButton; //前回からの間に放されたボタン
		unsigned downButton; //前回からの間に押されたボタン
		unsigned buttonState; //現在のボタンの状況
		unsigned movedAxis; //前回から変化があった軸
		float axis[8]; //各軸の値(-1.0〜+1.0)
	};
	static void AtMouse(const MOUSE_EVENT&);
	static void AtKey(const KEY_EVENT&);
	static void AtJS(const JS_EVENT&);


	static void DrawAll(const COMPLEX<4>&);
	static void DrawTransparentAll(const COMPLEX<4>&);


	//マウスイベント TODO:視線イベントを分離する
	virtual void OnMouseDown(const MOUSE_EVENT&){}; //ボタンが押された
	virtual void OnMouseUp(const MOUSE_EVENT&){}; //ボタンが放された
	virtual void OnMouseEnter(const MOUSE_EVENT&){}; //ポインタが窓に入った
	virtual void OnMouseMove(const MOUSE_EVENT&){}; //ポインタが窓の中を移動中
	virtual void OnMouseLeave(const MOUSE_EVENT&){}; //ポインタが窓から出た
	virtual void OnClick(const MOUSE_EVENT&){}; //クリックとその回数
	//キーイベント
	virtual void OnKeyDown(const KEY_EVENT&){}; //キーが押された
	virtual void OnKeyUp(const KEY_EVENT&){}; //キーが放された
	static unsigned long long keyState[];
	virtual void OnKeyChanged(const unsigned long long){};
	//ジョイスティック
	virtual void OnJSDown(const JS_EVENT&){};
	virtual void OnJSUp(const JS_EVENT&){};
	virtual void OnJSMove(const JS_EVENT&){};
	virtual void OnJSChange(const JS_EVENT&){};
	//コントロール
	virtual void OnResize(unsigned w, unsigned h){};
	virtual void OnFocused(){};
	virtual void OnUnfocused(){};
	virtual void OnEnter(WINDOW*){};
	virtual void OnLeave(){};


protected:
	/** 新規窓
	 * 新規に窓を精製する。
	 * 初期状態はフォーカスなし、テクスチャ未割り当て、不可視
	 * なので作ってから設定する必要がある。
	 */
	WINDOW();
	/** 新規窓(位置、サイズ付き)
	 * 新規に窓を精製する。
	 * 初期状態はフォーカスなし、テクスチャ未割り当て、不可視
	 * なので作ってから設定する必要がある。
	 */
	WINDOW(float h, float v, int wi, int hi);
	/** 中身付き新規窓
	 * 初期状態はフォーカスなし、テクスチャ割り当て済み、可視
	 * @attention h,vは左上ではなく中央
	 * @attention サイズはinitialImageから取得する
	 */
	WINDOW(float h, float v, const IMAGE&);

	virtual ~WINDOW();
private:
	typedef TOOLBOX::QUEUE<WINDOW> Q;

	static Q windowList;
	static Q invisibleWindowList;
	Q::NODE node;



	static WINDOW* focused;
	void Draw();
	GL::TEXTURE texture;


	struct POINT{
		float x;
		float y;
	};

	POINT position;
	unsigned width;
	unsigned height;


	static float motionDistance;
	static bool lookingFront;
	static POINT lookingPoint;
	static WINDOW* lookingWindow;
	static const float baseDistance;
	static float distance;

};

