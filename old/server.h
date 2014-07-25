
#pragma once

#include <view/view.h>
#include <util/socket.h>
#include <pthread.h>


class SERVER{
public:
	static bool Start(VIEW& v, const char* p);
private:
	static VIEW* view;
	static pthread_t listenerThread; //サーバソケットをacceptするだけのスレッド
	static void* Listener(void*);
	static bool be;
	SOCKET sock;
	pthread_t serverThread; //各サーバのスレッド
	SERVER(SERVERSOCKET&);
	static void* _ServerThread(void*);
	void ServerThread();
};

