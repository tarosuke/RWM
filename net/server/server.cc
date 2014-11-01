#include "server.h"

namespace NET{

	Server::Server(SERVERSOCKET& s, const Packet::Handler h[])
		: keep(true), server(s), handlers(h){
		//接続待ちのためだけのスレッドを起こす
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
		pthread_create(&thread, &attr, Thread, (void*)this);
	}

	void* Server::Thread(void* p){
		Server& s(*(Server*)p);
		while(s.keep){
			SOCKET* sock(new SOCKET(s.server));
			if(sock){
				//ノードを生成
				new ServerNode(*sock, s.handlers);
			}
		}
		return 0;
	}

}

