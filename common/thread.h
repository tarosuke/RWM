/**************************************************************** pthreadなオブジェクト
 * スレッドを使うオブジェクトは自己deleteできないため代わりに別スレッドでdeleteする
 * 子クラスでvoid Thread()を定義し、その中で処理を実行する。
 * Threadを抜けるとスレッド終了かつdelete。ただしdeleteは少し遅延する。
 * NOTE:セルフdeleteしないこと！
 */
#pragma once

#include <pthread.h>

#include "../toolbox/queue/queue.h"



class THREAD : public TOOLBOX::NODE<class THREAD>{
private:
protected:
	THREAD(); //スレッドを起こしてスレッドからThreadを呼ぶ。
	virtual void Thread()=0; //本処理:終了するとpthread_extiしdelete待ちになる。
	virtual ~THREAD(){};
private:
	//本体
	TOOLBOX::NODE<THREAD> node;
	pthread_t thread;
	static void* Igniter(void*); //スレッド開始用/終了時の処理

	//墓場
	static class CEMETARY{
	public:
		CEMETARY();
		void Register(TOOLBOX::NODE<THREAD>&);
	private:
		static pthread_t thread;
		static pthread_mutex_t igniter; //これが非0になったらdelete処理

		static TOOLBOX::QUEUE<THREAD> bodies;
		static void* Keeper(void*);
	}cemetary;
};

