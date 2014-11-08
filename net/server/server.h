#pragma once

#include "../net.h"
#include "../../common/thread.h"



namespace NET{

	class Server : public THREAD{
	public:
		Server(SERVERSOCKET&, const Packet::Handler[]);
	protected:
		void Close(){ keep = false; };
	private:
		class ServerNode : public Node{
		public:
			ServerNode(SOCKET& s, const Packet::Handler h[]) : Node(s, h){};
		};

		bool keep;
		SERVERSOCKET& server;
		const Packet::Handler* const handlers;

		//スレッド関連
		void Thread();
	};

}
