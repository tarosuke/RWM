#pragma once

#include <pthread.h>

#include "../net.h"



namespace NET{

	class Server{
	public:
		Server(SERVERSOCKET&, const Packet::Handler[]);
		~Server(){ keep = false; };
	private:
		class ServerNode : public Node{
		public:
			ServerNode(SOCKET& s, const Packet::Handler h[]) : Node(s, h){};
		};

		bool keep;
		SERVERSOCKET& server;
		const Packet::Handler* const handlers;

		//スレッド関連
		pthread_t thread; //接続待ちのためのスレッド
		static void* Thread(void*);
	};

}

//鯖はclientと違って接続時にスレッドを起こす(消滅処理はdereteしたあとでdeleteした人が)
