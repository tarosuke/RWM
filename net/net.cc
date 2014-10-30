

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

	void Packet::Send(SOCKET& sock){
		sock.Send((void*)&head, sizeof(Head));
		if(body){
			sock.Send(body, head.length);
		}
	}

	Packet::~Packet(){
		if(body){
			free(body);
		}
	}

	bool Packet::Handle(const Handler handlers[]){
		for(const Handler* h(handlers); (*h).type; ++h){
			if((*h).type == head.type){
				(*h).handler(*this);
				return true;
			}
		}
		return false;
	}


	Node::Node(SOCKET& s, const Packet::Handler h[]) : sock(s), handlers(h){
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
		pthread_create(&thread, &attr, Thread, (void*)this);
	}
	Node::~Node(){
		pthread_exit(0);
	}

	void* Node::Thread(void* p){
		Node& n(*(Node*)p);
		for(;;){
			Packet pack;
			pack.Receive(n.sock);
			if(!pack.Handle(n.handlers)){
				//TODO:デフォルトハンドラを呼ぶ
			}
		}
		return 0;
	}

}
