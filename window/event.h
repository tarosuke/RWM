
#pragma once



class WINDOW;
class EVENT{
	EVENT(const EVENT&);
	void operator=(const EVENT&);
public:
	unsigned timestamp;
	static const unsigned ShiftKey = 1;
	static const unsigned CtrlKey = 2;
	static const unsigned AltKey = 4;
	static unsigned modifiers; //モディファイアキーの状態
	const void* orgEvent;
	virtual void Handle(WINDOW&)const=0;
protected:
	EVENT(){};
	virtual ~EVENT(){};
};

class MOUSE_EVENT : public EVENT{
	MOUSE_EVENT(const MOUSE_EVENT&);
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
class MOUSE_DOWN_EVENT : public MOUSE_EVENT{
public:
	void Handle(WINDOW&)const;
};
class MOUSE_UP_EVENT : public MOUSE_EVENT{
public:
	void Handle(WINDOW&)const;
};
class MOUSE_MOVE_EVENT : public MOUSE_EVENT{
public:
	void Handle(WINDOW&)const;
};
class MOUSE_ENTER_EVENT : public MOUSE_EVENT{
public:
	void Handle(WINDOW&)const;
};
class MOUSE_LEAVE_EVENT : public MOUSE_EVENT{
public:
	void Handle(WINDOW&)const;
};

class KEY_EVENT : public EVENT{
public:
	unsigned charCode; //文字コード
	unsigned keyCode; //キーコード(あれば。なければ0)
};
class KEY_DOWN_EVENT : public KEY_EVENT{
public:
	void Handle(WINDOW&)const;
};
class KEY_REPEAT_EVENT : public KEY_EVENT{
public:
	void Handle(WINDOW&)const;
};
class KEY_UP_EVENT : public KEY_EVENT{
public:
	void Handle(WINDOW&)const;
};


class JS_EVENT : public EVENT{
	unsigned upButton; //前回からの間に放されたボタン
	unsigned downButton; //前回からの間に押されたボタン
	unsigned buttonState; //現在のボタンの状況
	unsigned movedAxis; //前回から変化があった軸
	float axis[8]; //各軸の値(-1.0〜+1.0)
};
