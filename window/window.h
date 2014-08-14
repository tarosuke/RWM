/******************************************************* window handler:window
 * この窓はXの窓ではない。
 * Xの窓には画面という概念があるが、この窓には窓があるだけで画面はない。
 * また、このクラスはバッキングストレージはテクスチャとしてしか持っていない。
 */
#pragma once

#include <string>

#include "../event.h"
#include "../toolbox/queue/queue.h"
#include "../toolbox/complex/complex.h"


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
	class MOUSE_DOWN_EVENT : public MOUSE_EVENT{
		void Handle()const{
		};
	};
};

