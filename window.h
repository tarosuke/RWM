/******************************************************* window handler:window
 * この窓はXの窓ではない。
 * Xの窓には画面という概念があるが、この窓には窓があるだけで画面はない。
 * また、このクラスはバッキングストレージはテクスチャとしてしか持っていない。
 * Rect単位でしか描き換えられないので別に描画用のバッファが必要。
 */
#pragma once


#include <GL/gl.h>
#include <GL/glx.h>

#include <toolbox/queue/queue.h>
#include <toolbox/complex/complex.h>
#include <toolbox/glpose/glpose.h>
#include <image.h>



class WINDOW{
	WINDOW(WINDOW&);
	void operator=(WINDOW&);
public:
	/** 全ての窓を描画する
	 * @attention このメソッドは何の準備もせずにいきなり描画する。
	 * @attention 描画先の設定が完了している事が前提。
	 */
	static void DrawAll(const GLPOSE&);

	//操作
	void Move(float h, float v); ///移動
	void Resize(const IMAGE&, unsigned w, unsigned h); ///リサイズ

protected:
	/** 新規窓
	 * 新規に窓を精製する。
	 * 初期状態はフォーカスなし、テクスチャ未割り当て、不可視
	 * なので作ってから設定する必要がある。
	 */
	WINDOW();
	/** 中身付き新規窓
	 * 初期状態はフォーカスなし、テクスチャ割り当て済み、可視
	 * @attention h,vは左上ではなく中央
	 * @attention サイズはinitialImageから取得する
	 */
	WINDOW(float h, float v, const IMAGE&);

	void AssignImage(const IMAGE&); ///テクスチャ割り当て、イメージ転送
	void UpdateImage( ///テクスチャ描き替え
	const IMAGE&, //元イメージ
	unsigned dx, //書き込み先座標
	unsigned dy,
	unsigned w, //転送サイズ
	unsigned h);
	void UpdateImage( ///テクスチャ描き替え
	const IMAGE&, //元イメージ
	unsigned sx, //元イメージ上の座標
	unsigned sy,
	unsigned dx, //書き込み先座標
	unsigned dy,
	unsigned w, //転送サイズ
	unsigned h);

	///イベントとハンドラ
	class EVENT{
	public:
		enum EVENT_TYPE{
			none,
			//マウス関連
			evMouseDown,
			evMouseUp,
			evMouseEnter,
			evMouseMove,
			evMouseLeave,
			evMouseClicked,
			//キーボード関連
			evKeyDown,
			evKeyRepeated,
			evKepUp,
			//ジョイスティック
			evJSDown,
			evJSUp,
			evJSMove,
			evJSChaged,
		}type;
		unsigned modifiers; //モディファイアキーの状態
	};
	///マウスイベント
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
	virtual void OnMouseDown(const MOUSE_EVENT&){}; //ボタンが押された
	virtual void OnMouseUp(const MOUSE_EVENT&){}; //ボタンが放された
	virtual void OnMouseEnter(const MOUSE_EVENT&){}; //ポインタが窓に入った
	virtual void OnMouseMove(const MOUSE_EVENT&){}; //ポインタが窓の中を移動中
	virtual void OnMouseLeave(const MOUSE_EVENT&){}; //ポインタが窓から出た
	virtual void OnClick(const MOUSE_EVENT&){}; //クリックとその回数
	///キーイベント
	class KEY_EVENT : public EVENT{
		unsigned charCode; //文字コード
		unsigned keyCode; //キーコード(あれば。なければ0)
	};
	virtual void OnKeyDown(const KEY_EVENT&){}; //キーが押された
	virtual void OnKeyRepeated(const KEY_EVENT&){}; //キーがオートリピートで押された
	virtual void OnKeyUp(const KEY_EVENT&){}; //キーが放された
	static unsigned long long keyState[];
	virtual void OnKeyChanged(const unsigned long long){};
	///ジョイスティック
	class JS_EVENT : public EVENT{
		unsigned upButton; //前回からの間に放されたボタン
		unsigned downButton; //前回からの間に押されたボタン
		unsigned buttonState; //現在のボタンの状況
		unsigned movedAxis; //前回から変化があった軸
		float axis[8]; //各軸の値(-1.0〜+1.0)
	};
	virtual void OnJSDown(const JS_EVENT&){};
	virtual void OnJSUp(const JS_EVENT&){};
	virtual void OnJSMove(const JS_EVENT&){};
	virtual void OnJSChange(const JS_EVENT&){};
	//描画
	virtual void OnRedraw(){}; //サイズ変更等で再描画が必要になった時(バックストレージ前提なので全体を再描画する)
	//コントロール
	virtual void OnFocused(){};
	virtual void OnUnfocused(){};

	virtual ~WINDOW(); //窓の場合基本的に自殺なのでデストラクタを直接呼ばない

private:
	void Draw(float xoff, float yoff, float distance);

	//窓全体関連
	static TOOLBOX::QUEUE<WINDOW> windowList;
	static const float scale; //窓表示スケール[m/px]
	static WINDOW* focused;
	static float baseDistance; //基準面の距離
	void Focus();
	void UnFocus();

	//単体窓制御関連
	TOOLBOX::NODE<WINDOW> node;
	unsigned tID; //窓の内容を保持するテクスチャID(0なら無効)
	bool visibility; //可視状態
	void SetVisibility(bool v){ visibility = v; };

	//位置とサイズ
	float horiz;
	float vert;
	float distance; //最後に描画した時の距離
	unsigned width;
	unsigned height;
};


