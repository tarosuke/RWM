/////////////////////////////////////////////////////////////// ABSTRACT EVENT
#pragma once


class Event{
	Event(const Event&);
	void operator=(const Event&);
public:

	class Listener{
		Listener();
		Listener(const Listener&);
		void operator=(const Listener&);
	public:
		virtual void HandleEvent(Event&)=0;
	protected:
		Listener(class Display&);
		virtual ~Listener();
	};


	virtual void Distribute(Listener&)=0;
protected:
	Event();
	virtual ~Event();
private:
};

/** ※仕様がかなり違うのでこのクラスは使えない。
 * 代わりにイベントの種類ごとに多態でハンドラが切り替わるようなイベントを窓に作る。
 */

