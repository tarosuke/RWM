/******************************************************* window handler:window
 * この窓はXの窓ではない。
 * Xの窓には画面という概念があるが、この窓には窓があるだけで画面はない。
 * また、このクラスはバッキングストレージはテクスチャとしてしか持っていない。
 * Rect単位でしか描き換えられないので別に描画用のバッファが必要。
 */
#pragma once

#include <toolbox/queue/queue.h>
#include <toolbox/complex/complex.h>
#include <image.h>



class WINDOW{
	WINDOW(WINDOW&);
	void operator=(WINDOW&);
public:
	/** 全ての窓を描画する
	 * @attention このメソッドは何の準備もせずにいきなり描画する。
	 * @attention 描画先の設定が完了している事が前提。
	 */
	static void DrawAll(const COMPLEX<4>&);

	//イベントと一次ハンドラ
	class EVENT{
	public:
		time_t timestamp;
		enum EVENT_TYPE{
			none,
			//マウス関連
			mouseDown,
			mouseUp,
			mouseEnter,
			mouseMove,
			mouseLeave,
			mouseClicked,
			//キーボード関連
			keyDown,
			keyRepeated,
			keyUp,
			//ジョイスティック
			jsDown,
			jsUp,
			jsMove,
			jsChaged,
		}type;
		unsigned modifiers; //モディファイアキーの状態
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

	void AssignImage(const IMAGE&); ///テクスチャ割り当て、イメージ転送
	void AssignImage(const void*, unsigned w, unsigned h);
	void UpdateImage( ///テクスチャ描き替え
		const IMAGE&, //元イメージ
		unsigned dx, //書き込み先座標
		unsigned dy,
		unsigned w, //転送サイズ
		unsigned h);
	void UpdateImage( ///IMAGEの一部でテクスチャ描き替え
		const IMAGE&, //元イメージ
		unsigned sx, //元イメージ上の座標
		unsigned sy,
		unsigned dx, //書き込み先座標
		unsigned dy,
		unsigned w, //転送サイズ
		unsigned h);

	//マウスイベント
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

	virtual ~WINDOW(); //窓の場合基本的に自殺なのでデストラクタを直接呼ばない

	//位置とサイズ
	float horiz;
	float vert;
	unsigned width;
	unsigned height;

	void SetVisibility(bool v){ visibility = v; };

private:
	void Draw(float xoff, float yoff, float distance);

	//窓全体関連
	static TOOLBOX::QUEUE<WINDOW> windowList;
	static const float scale; //窓表示スケール[m/px]
	static WINDOW* focused;
	static float baseDistance; //窓までの基本距離
	static float motionDistance; //表示する中心を決めるための仮想的な距離
	void Focus();
	void UnFocus();

	//単体窓制御関連
	TOOLBOX::NODE<WINDOW> node;
	unsigned tID; //窓の内容を保持するテクスチャID(0なら無効)
	bool visibility; //可視状態

	//見ている先
	static WINDOW* lookingWindow;
	static struct POINT{
		float x;
		float y;
	}lookingPoint;
};


