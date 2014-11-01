/**************************************************************** pthreadなオブジェクト
 * スレッドを使うオブジェクトは自己deleteできないため代わりに定期巡回でdeleteする
 */
#include "thread.h"


THREAD::CEMETARY THREAD::cemetary;


THREAD::THREAD() : node(*this){
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	pthread_create(&thread, &attr, Igniter, (void*)this);
}

void* THREAD::Igniter(void* p){
	THREAD& n(*(THREAD*)p);
	try{
		n.Thread();
	}
	catch(...){};
	cemetary.Register(n);
	return 0;
}




TOOLBOX::QUEUE<THREAD> THREAD::CEMETARY::bodies;
pthread_t THREAD::CEMETARY::thread;
pthread_mutex_t THREAD::CEMETARY::igniter;

THREAD::CEMETARY::CEMETARY(){
	//セマフォ初期化
	pthread_mutex_init(&igniter, 0);

	//スレッド起動
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	pthread_create(&thread, &attr, Keeper, (void*)this);
}

void THREAD::CEMETARY::Register(TOOLBOX::NODE<THREAD>& n){
	bodies.Add(n);
	pthread_mutex_unlock(&igniter);
}

void* THREAD::CEMETARY::Keeper(void*){
	for(;;){
		//bodyが登録されるまで待つ
		pthread_mutex_lock(&igniter);

		//bodyのdelete
		delete bodies.Get();
	}
	return 0;
}

