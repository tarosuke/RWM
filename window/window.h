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
#include "event.h"
#include "../image/image.h"


class WINDOW{
public:
	static void AtMouse(MOUSE_EVENT&);
	static void AtKey(KEY_EVENT&);
	static void AtJS(JS_EVENT&);


	static void UpdateAll(const COMPLEX<4>&);
	static void DrawAll();
	static void DrawTransparentAll();


	//マウスイベント TODO:視線イベントを分離する
	virtual void OnMouseDown(const MOUSE_EVENT&){}; //ボタンが押された
	virtual void OnMouseUp(const MOUSE_EVENT&){}; //ボタンが放された
	virtual void OnMouseEnter(const MOUSE_EVENT&){}; //ポインタが窓に入った
	virtual void OnMouseMove(const MOUSE_EVENT&){}; //ポインタが窓の中を移動中
	virtual void OnMouseLeave(const MOUSE_EVENT&){}; //ポインタが窓から出た
	virtual void OnClick(const MOUSE_EVENT&){}; //クリックとその回数
	//キーイベント
	virtual void OnKeyDown(const KEY_EVENT&){}; //キーが押された
	virtual void OnKeyRepeat(const KEY_EVENT&){}; //キーリピート
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
//	WINDOW();
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


	void SetVisibility(bool);
	bool IsVisible(){ return visible; };
	unsigned Width(){ return width; };
	unsigned Height(){ return height; };
	void Assign(const IMAGE&);
	void Update(const IMAGE&, int x, int y);
	void Move(float x, float y);
	void Resize(unsigned w, unsigned h);
private:
	typedef TOOLBOX::QUEUE<WINDOW> Q;

	static Q windowList;
	static Q invisibleWindowList;
	Q::NODE node;

	static WINDOW* focused;
	void Draw(float);
	GL::TEXTURE texture; //描画内容を保持
	bool visible;


	class POINT{
	public:
		const POINT operator-(const POINT& p) const{
			return (POINT){ x - p.x, y - p.y };
		};
		const POINT operator+(const POINT& p) const{
			return (POINT){ x + p.x, y + p.y };
		};
		float x;
		float y;
	};

	POINT position;
	unsigned width;
	unsigned height;

	POINT GetLocalPoint(const POINT&);

	static float motionDistance;
	static bool lookingFront;
	static POINT lookingPoint;
	static WINDOW* lookingWindow;
	static const float baseDistance;
	static float distance;
	static const float scale; //窓表示スケール[m/px]

};

