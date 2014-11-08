#include "server.h"


namespace NET{

	Server::Server(SERVERSOCKET& s, const Packet::Handler h[])
		: keep(true), server(s), handlers(h){}

	void Server::Thread(){
		while(keep){
			SOCKET* sock(new SOCKET(server));
			if(sock){
				//ノードを生成
				new ServerNode(*sock, handlers);
			}
		}
	}
}

