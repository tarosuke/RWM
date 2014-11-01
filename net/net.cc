

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


	Node::Node(SOCKET& s, const Packet::Handler h[])
		: sock(s), handlers(h), keep(true){}

	void Node::Close(){
		keep = false;
	}

	void Node::Thread(){
		try{
			while(keep){
				Packet pack;
				pack.Receive(sock);
				if(!pack.Handle(handlers)){
					//TODO:デフォルトハンドラを呼ぶ
				}
			}
		}
		catch(...){};
	}

}
