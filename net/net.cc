

#include <stdlib.h>

#include "net.h"
#include "socket.h"


namespace NET{

	unsigned Packet::sequence;

	Packet::Packet() :
		node(*this),
		head((Head){ 0, ++sequence, 0 }),
		result(0),
		body(0){}

	Packet::Packet(Type type, unsigned len, void* b) :
		node(*this),
		head((Head){ type, ++sequence, len }),
		result(0),
		body(malloc(len)){
		if(b){
			//元データが設定されているのでコピー
			memmove(body, b, len);
		}
	}

	void Packet::Receive(SOCKET& sock){
		//例外が出る時はソケットを閉じるときなのでcatchしない
		sock.Receive((void*)&head, sizeof(Head));
		if(body){
			free(body);
		}
		body = malloc(head.length);
		sock.Receive(body, head.length);
	}

	Packet::~Packet(){
		if(body){
			free(body);
		}
	}


	Node::Node(SOCKET& s, const Handler h[]) : sock(s), handlers(h){
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
		pthread_create(&thread, &attr, _Thread, (void*)this);
	}
	Node::~Node(){
		pthread_exit(0);
	}

	void* Node::_Thread(void* p){
		(*(Node*)p).Thread();
		return 0;
	}
	void Node::Thread(){
		//TODO:パケットを受信してハンドラを呼ぶ
		for(;;){
			Packet pack;
			pack.Receive(sock);
		}
	}

}
